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

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void startTCPServer(int port, int &server_sockfd, int &client_sockfd) {
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
   
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd < 0)
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
   
    if (bind(server_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(server_sockfd, 5);
    clilen = sizeof(cli_addr);
    client_sockfd = accept(server_sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (client_sockfd < 0){
        error("ERROR on accept");
    }
}

void startTCPClient(const char *hostname, int port, int &sockfd) {
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(hostname);
    if (server == NULL) {
        std::cerr << "ERROR, no such host\n";
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, (size_t)server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");
}

void handleCommunication(int input_fd, int output_fd) {
    char buffer[256];
    ssize_t n;

    while (true) {
        bzero(buffer, 256);
        n = read(input_fd, buffer, 255);
        if (n <= 0) break;  // Exit loop if connection is closed or error occurs
        n = write(output_fd, buffer, strlen(buffer));
        if (n < 0) error("ERROR writing to socket");
    }
}

int main(int argc, char *argv[]) {
    int input_sockfd = -1, output_sockfd = -1;
    int server_sockfd = -1, client_sockfd = -1;
    std::string exec_command;
    bool input_set = false, output_set = false, both_set = false;

    for (size_t i = 0; i < argc; ++i) {
        if (std::string(argv[i]) == "-e" && i + 1 < argc) {
            exec_command = argv[++i];
        } else if (std::string(argv[i]).substr(0, 5) == "-iTCP") {
            int port = std::stoi(std::string(argv[i]).substr(6));
            startTCPServer(port, server_sockfd, client_sockfd);
            input_sockfd = client_sockfd;
            input_set = true;
        } else if (std::string(argv[i]).substr(0, 5) == "-oTCP") {
            std::string connection = std::string(argv[i]).substr(6);
            size_t comma_pos = connection.find(',');
            std::string hostname = connection.substr(0, comma_pos);
            int port = std::stoi(connection.substr(comma_pos + 1));
            startTCPClient(hostname.c_str(), port, output_sockfd);
            output_set = true;
        } else if (std::string(argv[i]).substr(0, 5) == "-bTCP") {
            int port = std::stoi(std::string(argv[i]).substr(6));
            startTCPServer(port, server_sockfd, client_sockfd);
            input_sockfd = client_sockfd;
            output_sockfd = client_sockfd;
            both_set = true;
        }
    }

    if (exec_command.empty()) {
        std::cerr << "No execution command provided.\n";
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        if (input_set || both_set) {
            dup2(input_sockfd, STDIN_FILENO);
            close(input_sockfd);
        }
        if (output_set || both_set) {
            dup2(output_sockfd, STDOUT_FILENO);
            close(output_sockfd);
        }

        execlp("/bin/sh", "sh", "-c", exec_command.c_str(), (char *)0);
        std::cerr << "Failed to execute " << exec_command << "\n";
        return 1;
    } else if (pid > 0) {
        if (input_set || both_set) {
            close(server_sockfd);
            handleCommunication(input_sockfd, STDOUT_FILENO);
            close(input_sockfd);
        }
        if (output_set || both_set) {
            handleCommunication(STDIN_FILENO, output_sockfd);
            close(output_sockfd);
            waitpid(pid, NULL, 0);   
        }
    } else {
        std::cerr << "Failed to fork\n";
        return 1;
    }

    return 0;
}