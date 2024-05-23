#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
using namespace std;

#define MAX_COMMAND_LENGTH 1000

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "-e") != 0) {
        cout << "Usage: " << argv[0] << " -e <command>" << endl;
        return 1;
    }
    pid_t pid = fork();

    if (pid == 0) {
        // זהו התהליך הילד
        execlp("ttt", "ttt", "123456789", nullptr);
        // אם הגענו לכאן, execlp נכשל
        std::cerr << "Failed to execute ttt\n";
        return 1;
    } else if (pid > 0) {
        // זהו התהליך האב
        wait(nullptr); // מחכה לסיום התהליך הילד
    } else {
        std::cerr << "Failed to fork\n";
        return 1;
    }
    cout << "test" << endl;
    return 0;
}
