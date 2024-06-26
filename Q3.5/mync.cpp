#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <csignal>
#include <sys/wait.h>
#include <sstream>
#include <string>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int stringToInt(const std::string& str) {
    std::istringstream iss(str);
    int value;
    if (!(iss >> value)) {
        // Handle the conversion failure here
        std::cerr << "Error: Invalid integer string - " << str << std::endl;
        exit(1);
    }
    return value;
}

void startTCPServer(int port, int &server_sockfd, int &client_sockfd) {
    std::cout << "Starting TCP Server..." << std::endl;

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0); // Creating a socket
    if (server_sockfd < 0) {
        error("Error: opening socket");
    }
    std::cout << "Socket is created successfully!" << std::endl;

    // Set SO_REUSEADDR socket option
    int opt = 1;
    if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        error("Error: setting socket option");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(server_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error: on binding");
    }
    std::cout << "bind() success!" << std::endl;

    listen(server_sockfd, 5);
    std::cout << "listening..." << std::endl;
    clilen = sizeof(cli_addr);
    client_sockfd = accept(server_sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (client_sockfd < 0) {
        error("Error: on accept");
    }
    std::cout << "accept() success!" << std::endl;
}

void startTCPClient(const char *hostname, int port, int &sockfd) {
    std::cout << "Starting TCP Client..." << std::endl;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Creating socket
    if (sockfd < 0) {
        error("Error: opening socket");
    }
    std::cout << "Socket is created successfully!" << std::endl;

    server = gethostbyname(hostname);
    if (server == NULL) {
        std::cerr << "Error: no such host" << std::endl;
        exit(0);
    }
    // Set SO_REUSEADDR socket option
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        error("Error: setting socket option");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, (size_t)server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error: connecting");
    }
    std::cout << "connect() success!" << std::endl;
}

void handleCommunication(int input_fd, int output_fd) {
    char buffer[256];
    ssize_t n;

    while (true) {
        bzero(buffer, 256);
        n = read(input_fd, buffer, 255);
        if (n <= 0) break;  // Exit loop if connection is closed or error occurs
        n = write(output_fd, buffer, strlen(buffer));
        if (n < 0) error("Error: writing to socket");
    }
}
int main(int argc, char *argv[]) {
    int input_sockfd = -1, output_sockfd = -1;
    int server_sockfd = -1, client_sockfd = -1;
    std::string exec_command;
    bool input_set = false, output_set = false, both_set = false;

     for (int i = 1; i < argc; ++i) { // Start from 1 to skip the program name
        if (std::string(argv[i]) == "-e" && i + 1 < argc) {
            exec_command = argv[++i];
        } else if (std::string(argv[i]).substr(0, 5) == "-bTCP") {
            int port = std::stoi(std::string(argv[i]).substr(5));
            startTCPServer(port, server_sockfd, client_sockfd);
            input_sockfd = client_sockfd;
            output_sockfd = client_sockfd;
            both_set = true;
        }
    }

    if (exec_command.empty()) {
        std::cerr << "No execution command provided." << std::endl;
        return 1;
    }

    if (both_set) {
        fflush(stdout);
        close(1);  // Close stdout
        if (dup2(output_sockfd, STDOUT_FILENO) < 0) {
            std::cerr << "Failed to redirect stdout" << std::endl;
            exit(1);
        }
        close(output_sockfd);
    }

    // Prepare arguments for execvp
    const char *cmd = exec_command.c_str();
    const char *args[] = {"/bin/sh", "-c", cmd, NULL};

    // Execute the command
    int ececvpNum = execvp(args[0], const_cast<char *const *>(args));
    if(ececvpNum == -1){
        std::cerr << "Failed to execute " << exec_command << std::endl;
        exit(1);
    }

    return 0;
}
