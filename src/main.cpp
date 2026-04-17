#include "board.h"
#include "uci.h"
#include <iostream>
#include <sstream>

using namespace std;

int main() {
    initZobrist();
    string firstLine;
    if (getline(cin, firstLine)) {
        stringstream ss(firstLine);
        string cmd;
        ss >> cmd;

        if (cmd == "uci") {
            runUciMode();
        } else {
            runConsoleGame();
        }
    } else {
        runConsoleGame();
    }

    return 0;
}