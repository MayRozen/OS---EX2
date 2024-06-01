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
#include <filesystem>

using namespace std;
//namespace fs = std::filesystem;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void alarm_handler(int signum) {
    exit(0);
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

    unlink(path.c_str());  // Unlink the path before binding
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sockfd);
        unlink(path.c_str());
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
        unlink(path.c_str());
        error("ERROR connecting");
    }

    char buffer[256];
    while (true) {
        std::cin.getline(buffer, 256);
        int n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            close(sockfd);
            unlink(path.c_str());
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
            unlink(path.c_str());
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

    unlink(path.c_str());  // Unlink the path before binding
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sockfd);
        unlink(path.c_str());
        error("ERROR on binding");
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    if ((newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen)) < 0) {
        close(sockfd);
        unlink(path.c_str());
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

     // Use a subdirectory for Unix domain sockets
    std::string socket_dir = "/tmp/unix_sockets/";
    // std::filesystem create_directories(socket_dir);
    std::string socket_path = socket_dir + path;

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