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
#include <cstdlib>
#include <sys/un.h>

using namespace std;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void alarm_handler(int signum) {
    exit(0);
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

void handle_unix_domain_server_datagram(const std::string& path) {
    int sockfd;
    struct sockaddr_un serv_addr;

    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
        error("ERROR opening socket");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, path.c_str(), sizeof(serv_addr.sun_path) - 1);

    unlink(path.c_str());
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sockfd);
        error("ERROR on binding");
    }

    char buffer[256];
    while (true) {
        memset(buffer, 0, 256);
        int n = recvfrom(sockfd, buffer, 255, 0, NULL, NULL);
        if (n < 0) {
            close(sockfd);
            error("ERROR on recvfrom");
        }
        std::cout << buffer << std::endl;
    }
}

void handle_unix_domain_client_stream(const std::string& path) {
    int sockfd;
    struct sockaddr_un serv_addr;

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        error("ERROR opening socket");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, path.c_str(), sizeof(serv_addr.sun_path) - 1);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sockfd);
        error("ERROR connecting");
    }

    char buffer[256];
    while (true) {
        std::cin.getline(buffer, 256);
        int n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            close(sockfd);
            error("ERROR writing to socket");
        }
    }
}


void handle_unix_domain_client_datagram(const std::string& path) {
    int sockfd;
    struct sockaddr_un serv_addr;

    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
        error("ERROR opening socket");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, path.c_str(), sizeof(serv_addr.sun_path) - 1);

    char buffer[256];
    while (true) {
        std::cin.getline(buffer, 256);
        int n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        if (n < 0) {
            close(sockfd);
            error("ERROR on sendto");
        }
    }
}

void handle_unix_domain_server_stream(const std::string& path) {
    int sockfd, newsockfd;
    struct sockaddr_un serv_addr, cli_addr;
    socklen_t clilen;

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        error("ERROR opening socket");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, path.c_str(), sizeof(serv_addr.sun_path) - 1);

    unlink(path.c_str());
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sockfd);
        error("ERROR on binding");
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    if ((newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen)) < 0) {
        close(sockfd);
        error("ERROR on accept");
    }

    char buffer[256];
    while (true) {
        memset(buffer, 0, 256);
        int n = read(newsockfd, buffer, 255);
        if (n < 0) {
            close(newsockfd);
            close(sockfd);
            error("ERROR reading from socket");
        }
        std::cout << buffer << std::endl;
    }
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
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " -e <command> [options]" << std::endl;
        return 1;
    }

    std::string mode = argv[3];
    std::string path_or_address = argv[4];
    cout << "Mode: " << mode << endl;
    cout << "Path or Address: " << path_or_address << endl;

   int input_sockfd = -1, output_sockfd = -1;
    int server_sockfd = -1, client_sockfd = -1;
    std::string exec_command;
    bool input_set = false, output_set = false, both_set = false;

    for (int i = 1; i < argc; ++i) { // Start from 1 to skip the program name
        if (std::string(argv[i]) == "-e" && i + 1 < argc) {
            exec_command = argv[++i];
        } 
        else if (std::string(argv[i]).substr(0, 5) == "UDSSD") {
            int port = std::stoi(std::string(argv[i]).substr(5));
            startTCPServer(port, server_sockfd, client_sockfd);
            input_sockfd = client_sockfd;
            input_set = true;
            printf("input socket = %d\n",input_sockfd);
        } 
        else if (std::string(argv[i]).substr(0, 5) == "UDSCD") {
            std::string connection = std::string(argv[i]).substr(5); // Fix the index here
            size_t comma_pos = connection.find(',');
            std::string hostname = connection.substr(0, comma_pos);
            int port = std::stoi(connection.substr(comma_pos + 1));
            output_set = true;
        } 
        else if (std::string(argv[i]).substr(0, 5) == "UDSSD") {
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

    else if (mode == "-i" && path_or_address.rfind("UDSSD", 0) == 0) {
        handle_unix_domain_server_datagram(path_or_address.substr(5));
    }

    else if (mode == "-o" && path_or_address.rfind("UDSCD", 0) == 0) {
        handle_unix_domain_client_datagram(path_or_address.substr(5));
    } 

    else if (mode == "-i" && path_or_address.rfind("UDSSS", 0) == 0) {
        handle_unix_domain_server_stream(path_or_address.substr(5));
    } 
    
    else if (mode == "-o" && path_or_address.rfind("UDSCS", 0) == 0) {
        handle_unix_domain_client_stream(path_or_address.substr(5));
    } 
    else {
        std::cerr << "Invalid mode or path" << std::endl;
        return 1;
    }

    return 0;
}