#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

#define MAX_COMMAND_LENGTH 1000

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "-e") != 0) {
        cout << "Usage: " << argv[0] << " -e <command>" << endl;
        return 1;
    }
    char command[MAX_COMMAND_LENGTH];
    strcpy(command, argv[2]); // Copy the command to execute
    FILE *fp;
    fp = popen(command, "r"); // Open a pipe to execute the command
    if (fp == nullptr) {
        std::cout << "Error executing command." << std::endl;
        return 1;
    }
    char buffer[MAX_COMMAND_LENGTH];
    while (fgets(buffer, MAX_COMMAND_LENGTH, fp) != nullptr) {
        cout << buffer; // Print output from the executed command
    }
    
    pclose(fp); // Close the pipe
    
    return 0;
}
