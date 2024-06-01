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
    return;
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
    return;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <mode> <path>" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    std::string path = argv[2];

    cout << "mode: " <<  mode << endl;
    cout << "path: " << path << endl;

    if (mode == "-i" && path.rfind("UDSSD", 0) == 0) {
        handle_unix_domain_server_datagram(path.substr(5));
    } 
    else if (mode == "-o" && path.rfind("UDSCD", 0) == 0) {
        handle_unix_domain_client_datagram(path.substr(5));
    } 
    else if (mode == "-i" && path.rfind("UDSSS", 0) == 0) {
        handle_unix_domain_server_stream(path.substr(5));
    } 
    else if (mode == "-o" && path.rfind("UDSCS", 0) == 0) {
        handle_unix_domain_client_stream(path.substr(5));
    } 
    else {
        std::cerr << "Invalid mode or path" << std::endl;
        return 1;
    }
    return 0;
}