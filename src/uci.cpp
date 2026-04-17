#include "uci.h"
#include "board.h"
#include "movegen.h"
#include "search.h"
#include <iostream>
#include <sstream>
#include <climits>

using namespace std;

string moveToString(Move m) {
    string res = "";
    res += (char)(m.fc + 'a');
    res += (char)(8 - m.fr + '0');
    res += (char)(m.tc + 'a');
    res += (char)(8 - m.tr + '0');
    return res;
}

void printBoard() {
    cout << "\n  +-----------------+" << endl;
    for (int r = 0; r < 8; r++) {
        cout << (8 - r) << " | ";
        for (int c = 0; c < 8; c++) {
            Piece p = board[r][c]; char symb = '.';
            switch(p) {
                case WP: symb = 'P'; break; case WN: symb = 'N'; break;
                case WB: symb = 'B'; break; case WR: symb = 'R'; break;
                case WQ: symb = 'Q'; break; case WK: symb = 'K'; break;
                case BP: symb = 'p'; break; case BN: symb = 'n'; break;
                case BB: symb = 'b'; break; case BR: symb = 'r'; break;
                case BQ: symb = 'q'; break; case BK: symb = 'k'; break;
            }
            cout << symb << " ";
        }
        cout << "|" << endl;
    }
    cout << "  +-----------------+\n    a b c d e f g h\n" << endl;
}


void runUciMode() {
    cout << "id name ZabavaEngine 1.0" << endl;
    cout << "id author Zabava" << endl;
    cout << "uciok" << endl;

int lastScore = 0;
int alpha = -INT_MAX;
int beta = INT_MAX;
int window = 50;

for (int d = 1; d <= 20; d++) {
    if (d > 1) {
        alpha = lastScore - window;
        beta = lastScore + window;
    }

    int score = alphaBeta(d - 1, 0, -INT_MAX, INT_MAX, !whiteToMove, true);

    if (score <= alpha || score >= beta) {
        score = alphaBeta(d, 0, -INT_MAX, INT_MAX, true, true);
    }
    lastScore = score;
}
    string line, token;
    while (getline(cin, line)) {
        stringstream ss(line);
        ss >> token;

        if (token == "isready") {
            cout << "readyok" << endl;
        }
        else if (token == "uci") {
            cout << "uciok" << endl;
        }
        else if (token == "position") {
            string sub; ss >> sub;
            if (sub == "startpos") {
                initBoard();
                ss >> sub;
            } else if (sub == "fen") {
                initBoard();
            }

            while (ss >> sub) {
                if (sub == "moves") continue;
                vector<Move> ms = generateLegalMoves();
                for(auto& m : ms) {
                    if (moveToString(m) == sub) {
                        makeMove(m);
                        break;
                    }
                }
            }
        }
        else if (token == "go") {

            int wtime = -1, btime = -1, winc = 0, binc = 0;
            string param;
            while(ss >> param) {
                if(param == "wtime") ss >> wtime;
                else if(param == "btime") ss >> btime;
                else if(param == "winc") ss >> winc;
                else if(param == "binc") ss >> binc;
            }

            int myTime = whiteToMove ? wtime : btime;
            int myInc = whiteToMove ? winc : binc;


            if (myTime != -1) {
                allocatedTime = (myTime / 25) + (myInc / 2);
                if (allocatedTime < 50) allocatedTime = 50;
                if (allocatedTime > myTime - 500) allocatedTime = myTime - 500;
                if (allocatedTime < 1) allocatedTime = 1;
            } else {
                allocatedTime = 1000;
            }

            vector<Move> ms = generateLegalMoves();
            if (ms.empty()) continue;

            Move bestMoveSoFar = ms[0];
            searchStartTime = chrono::steady_clock::now();
            stopSearch = false;

            for (int d = 1; d <= 20; d++) {
                int bestScore = whiteToMove ? -200000 : 200000;
                Move bestInIteration = ms[0];

                for (auto& m : ms) {
                    Undo u = makeMove(m);
                  int s = alphaBeta(d - 1, 1, -INT_MAX, INT_MAX, !whiteToMove, true);

                  undoMove(m, u);
                    if (stopSearch) break;

                    if (whiteToMove) {
                        if (s > bestScore) { bestScore = s; bestInIteration = m; }
                    } else {
                        if (s < bestScore) { bestScore = s; bestInIteration = m; }
                    }
                }

                if (stopSearch) break;

                bestMoveSoFar = bestInIteration;

                auto now = chrono::steady_clock::now();
                long long elapsed = chrono::duration_cast<chrono::milliseconds>(now - searchStartTime).count();
                cout << "info depth " << d << " score cp " << bestScore << " time " << elapsed << " pv " << moveToString(bestMoveSoFar) << endl;

                if (elapsed > (allocatedTime * 0.6)) break;
            }
            cout << "bestmove " << moveToString(bestMoveSoFar) << endl;
        }
        else if (token == "quit") break;
    }
}

void runConsoleGame() {
    initBoard();
    cout << "=== Consol Game ===" << endl;
    cout << "Enter moves in e2e4 format (or ‘exit’ to quit)" << endl;


    while (true) {
        printBoard();

        if (whiteToMove) {
            cout << "White's move: ";
        } else {
            cout << "Black move: ";
        }

        vector<Move> legalMoves = generateLegalMoves();
        if (legalMoves.empty()) {
            if (inCheck(whiteToMove)) cout << "CHECK! The game is over" << endl;
            else cout << "PAT! Draw." << endl;
            break;
        }

        string userMove;
        cin >> userMove;
        if (userMove == "exit") break;

        bool moveFound = false;
        for (auto& m : legalMoves) {
            if (moveToString(m) == userMove) {
                makeMove(m);
                moveFound = true;
                break;
            }
        }

        if (!moveFound) {
            cout << "Error: This is not possible! Please try again" << endl;
            continue;
        }

        printBoard();
        cout << "Komp dumaet..." << endl;
        legalMoves = generateLegalMoves();
        if (legalMoves.empty()) break;

        Move bestBotMove = legalMoves[0];
        int bestVal = 1000000;

        for(auto& m : legalMoves) {
            Undo u = makeMove(m);
            int val = alphaBeta(5, 0, -INT_MAX, INT_MAX, true, true);
            undoMove(m, u);
            if (val < bestVal) { bestVal = val; bestBotMove = m; }
        }
        makeMove(bestBotMove);
        cout << "Komp poshel: " << moveToString(bestBotMove) << endl;

    }
}