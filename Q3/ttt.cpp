#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <cstring>

using namespace std;

// Checking if there is a winner
bool checkWin(const vector<char>& gameBoard, char player) {
    // Checking rows, cols and diagonals
    return (gameBoard[0] == player && gameBoard[1] == player && gameBoard[2] == player) ||
           (gameBoard[3] == player && gameBoard[4] == player && gameBoard[5] == player) ||
           (gameBoard[6] == player && gameBoard[7] == player && gameBoard[8] == player) ||
           (gameBoard[0] == player && gameBoard[3] == player && gameBoard[6] == player) ||
           (gameBoard[1] == player && gameBoard[4] == player && gameBoard[7] == player) ||
           (gameBoard[2] == player && gameBoard[5] == player && gameBoard[8] == player) ||
           (gameBoard[0] == player && gameBoard[4] == player && gameBoard[8] == player) ||
           (gameBoard[2] == player && gameBoard[4] == player && gameBoard[6] == player);
}

// Checking the comStrategy
bool isValidStrategy(const string& comStrategy) {
    if (comStrategy.length() != 9) { // Must be exactly 9
        return false;
    }
    unordered_set<char> digits;
    for (char c : comStrategy) { // Checking if all the digits are between 1 and 9
        if (c < '1' || c > '9') {
            return false;
        }
        digits.insert(c);
    }
    return digits.size() == 9;
}

// Checking the next turn of the computer
int findNextMove(const vector<char>& gameBoard, const string& comStrategy) {
    for (char c : comStrategy) {
        size_t pos = (size_t)(c - '1');
        if (gameBoard[pos] == ' ') {
            return pos;
        }
    }
    return -1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Error: incorrect number of arguments" << endl;
        return -1;
    }

    string comStrategy = argv[1];
    if (!isValidStrategy(comStrategy)) { // Checking if the strategy meets the requirements
        cout << "Error: strategy is invalid" << endl;
        return -1;
    }

    if (strlen(argv[1]) != 9) {
        cout << "Error: input is too long" << endl;
        return -1;
    }

    vector<char> gameBoard(9, ' ');  // Creating the game board and initializing it with spaces
    char computer = 'X';
    char human = 'O';

    // The game starts and the computer makes the first move
    size_t move = (size_t)comStrategy[0] - '1'; // The first position from the strategy
    gameBoard[move] = computer; // Mark the computer's move on the game board
    cout << move + 1 << endl; // Output the position of the move

    while (true) {
        size_t humanMove;
        cin >> humanMove;
        if (humanMove < 1 || humanMove > 9) {
            cout << "Error: human move is invalid" << endl;
            return -1;
        }
        humanMove -= 1;

        if (gameBoard[humanMove] != ' ') {
            cout << "Error: position is taken" << endl;
            return 0;
        }

        gameBoard[humanMove] = human; // After every turn, check if there is a winner
        if (checkWin(gameBoard, human)) {
            cout << "I lost" << endl;
            return 0;
        }

        move = (size_t)findNextMove(gameBoard, comStrategy);
        if (move == -1) {
            cout << "DRAW" << endl;
            return 0;
        }

        gameBoard[move] = computer;
        cout << move + 1 << endl;
        
        if (checkWin(gameBoard, computer)) {
            cout << "I win" << endl;
            return 0;
        }
    }

    return 0;
}
