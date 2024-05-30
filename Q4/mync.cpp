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

using namespace std;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void alarm_handler(int signum) {
    exit(0);
}

void startUDPServer(int port, int &server_sockfd,string argv) {
    cout << "Starting UDP Server..." << endl;

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    server_sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Creating a UDP socket
    if (server_sockfd < 0) {
        error("Error: opening socket");
    }
    cout << "Socket is created successfully!" << endl;

    // Set SO_REUSEADDR socket option
    int opt = 1;
    if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        error("Error: setting socket option");
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(server_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error: on binding");
    }
    cout << "bind() success!" << endl;
    return;
}

void startTCPClient(const char *hostname, int port, int &sockfd) {
    cout << "Starting TCP Client..." << endl;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Creating socket
    if (sockfd < 0) {
        error("Error: opening socket");
    }
    cout << "Socket is created successfully!" << endl;

    server = gethostbyname(hostname);
    if (server == NULL) {
        cerr << "Error: no such host" << endl;
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
    cout << "connect() success!" << endl;
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
    int server_sockfd = -1;
    std::string exec_command;
    bool input_set = false, output_set = false, both_set = false;

    for (int i = 1; i < argc; ++i) { // Start from 1 to skip the program name
        if (std::string(argv[i]) == "-e" && i + 1 < argc) {
            exec_command = argv[++i];
        } 
        else if (std::string(argv[i]).substr(0, 5) == "-iUDP") {
            int port = std::stoi(std::string(argv[i]).substr(5));
            startUDPServer(port, server_sockfd,exec_command);
            input_sockfd = server_sockfd;
            input_set = true;
        } 
        else if (std::string(argv[i]).substr(0, 5) == "-bUDP") {
            int port = std::stoi(std::string(argv[i]).substr(5));
            startUDPServer(port, server_sockfd,exec_command);
            input_sockfd = server_sockfd;
            output_sockfd = server_sockfd;
            both_set = true;
        }
    }

    if (exec_command.empty()) {
        cerr << "No execution command provided." << endl;
        return 1;
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (input_set || both_set) {
            close(0);
            if (dup2(input_sockfd, STDIN_FILENO) < 0) {
                cerr << "Failed to redirect stdin" << endl;
                exit(1);
            }
            close(input_sockfd);
        }
        if (output_set || both_set) {
            if (dup2(output_sockfd, STDOUT_FILENO) < 0) {
                cerr << "Failed to redirect stdout" << endl;
                exit(1);
            }
            close(output_sockfd);
        }
        execlp("/bin/sh", "sh", "-c", exec_command.c_str(), (char *)0);
        cerr << "Failed to execute " << exec_command << endl;
        exit(1);
    } 
    else if (pid > 0) {
        // Parent process
        if (input_set || both_set) {
            close(server_sockfd);
            close(input_sockfd);
        }
        if (output_set || both_set) {
            close(output_sockfd);
        }
        waitpid(pid, NULL, 0);
    } 
    else {
        cerr << "Failed to fork" << endl;
        exit(1);
    }
    return 0;
}
