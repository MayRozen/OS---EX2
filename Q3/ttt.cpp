
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
    if (comStrategy.length() != 9){ // More or less from 9
         return false;
    }
    unordered_set<char> digits;
    for (char c : comStrategy) { // Checking if all the digits is between 1 to 9
        if (c < '1' || c > '9'){
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
        if (gameBoard[pos] == false) {
            return pos;
        }
    }
    return -1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Error: too much arguments" << endl;
        return 1;
    }

    string comStrategy = argv[1];
    if (!isValidStrategy(comStrategy)) { // Checking if the strategy meets the requirments
        cout << "Error: strategy is invalid" << endl;
        return 1;
    }

    if(strlen(argv[1])!=9){
        cout << "Error: input is too long" << endl;
        return 1;
    }

    vector<char> gameBoard(9, false);  // Creating the game board and resseting its values to false
    char computer = 'X';
    char human = 'O';

    // The game start and the computer do the first turn
    size_t move = (size_t)comStrategy[0] - '1'; // The MSB from the input
    gameBoard[move] = computer; // Sign the computer's turn on the game board
    cout << move + 1 << endl; // Move forward the next possition on the board

    while (true) {
        size_t humanMove;
        cin >> humanMove;
        humanMove -= 1;

        if(humanMove==1){
            humanMove++;
        }

        if (humanMove < 1 || humanMove > 9) {
            cout << humanMove << endl;
            cout << "Error: human Move is invalid" << endl;
            return 1;
        }

        gameBoard[humanMove] = human; // After every turn check if there is a winner
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