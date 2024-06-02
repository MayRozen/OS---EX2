#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <csignal>
#include <sys/wait.h>
#include <sys/un.h>
#include <filesystem>

using namespace std;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void handle_unix_domain_client_stream(const std::string& path) {
    int sockfd;
    struct sockaddr_un serv_addr;

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        error("ERROR opening socket");
    }
    cout << "stream socket() success! " << path << endl;

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, path.c_str(), sizeof(serv_addr.sun_path) - 1);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sockfd);
        unlink(path.c_str());
        error("ERROR connecting");
    }
    cout << "stream connect() success! " << path << endl;

    char buffer[256];
    while (true) { // one game
        // Send numbers to server
        int number;
        cout << "Enter 9 numbers: ";
        cin >> number;
        std::string number_str = std::to_string(number);
        int n = write(sockfd, number_str.c_str(), number_str.length());
        if (n < 0) {
            close(sockfd);
            unlink(path.c_str());
            error("ERROR writing to socket");
        }
        // Execute ttt program with the received number as input
        std::string command = "./ttt ";
        command += std::to_string(number); // Convert the number to string and append to the command
        system(command.c_str());
    }

    close(sockfd);
}


void handle_unix_domain_server_stream(const std::string& path) {
    int sockfd, newsockfd;
    struct sockaddr_un serv_addr, cli_addr;
    socklen_t clilen;

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        error("ERROR opening socket");
    }
    cout << "stream socket() success!" << path << endl;

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, path.c_str(), sizeof(serv_addr.sun_path) - 1);

    unlink(path.c_str());  // Unlink the path before binding
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sockfd);
        unlink(path.c_str());
        error("ERROR on binding");
    }
    cout << "stream bind() success!" << path << endl;

    listen(sockfd, 5);
    cout << "stream listening..." << path << endl;
    clilen = sizeof(cli_addr);
    if ((newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen)) < 0) {
        close(sockfd);
        unlink(path.c_str());
        error("ERROR on accept");
    }
    cout << "stream accept() success!" << path << endl;

    char buffer[256];
    while (true) {
        // Receive numbers from client
        int number;
        int n = read(newsockfd, buffer, sizeof(buffer));
        if (n > 0) {
            buffer[n] = '\0';
            // Convert the received string to a number
            number = std::stoi(buffer);
            cout << "Received 9 numbers from client: " << number << endl;

            
        } else {
            break;
        }
    }

    close(newsockfd);
    close(sockfd);
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
    std::string socket_path = socket_dir + path;

    if (mode == "-i" && path.rfind("UDSSD", 0) == 0) {
            handle_unix_domain_server_stream(path.substr(5));
            
            
        } 
        else if (mode == "-o" && path.rfind("UDSCD", 0) == 0) {
            handle_unix_domain_client_stream(path.substr(5));
        } 
        else {
            std::cerr << "Invalid mode or path" << std::endl;
            return 1;
        }
    return 0;
}
    
