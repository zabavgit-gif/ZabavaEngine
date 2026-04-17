#include "tests.h"
#include "board.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <cctype>
std::string moveToString(Move m);

using namespace std;
struct PerftTest {
    string name;
    string fen;
    vector<unsigned long long> results;
};


void clearBoard() {
    for(int r=0; r<8; r++)
        for(int c=0; c<8; c++)
            board[r][c] = EMPTY;
}


void loadFen(const string& fen) {
    clearBoard();
    stringstream ss(fen);
    string pos, side, castling, enPassant;
    ss >> pos >> side >> castling >> enPassant;

    int r = 0, c = 0;
    for (char ch : pos) {
        if (ch == '/') { r++; c = 0; }
        else if (isdigit(ch)) { c += ch - '0'; }
        else {
            if (ch == 'P') board[r][c] = WP;
            else if (ch == 'N') board[r][c] = WN;
            else if (ch == 'B') board[r][c] = WB;
            else if (ch == 'R') board[r][c] = WR;
            else if (ch == 'Q') board[r][c] = WQ;
            else if (ch == 'K') board[r][c] = WK;
            else if (ch == 'p') board[r][c] = BP;
            else if (ch == 'n') board[r][c] = BN;
            else if (ch == 'b') board[r][c] = BB;
            else if (ch == 'r') board[r][c] = BR;
            else if (ch == 'q') board[r][c] = BQ;
            else if (ch == 'k') board[r][c] = BK;
            c++;
        }
    }

    whiteToMove = (side == "w");

    kingMoved[0] = kingMoved[1] = true;
    rookLMoved[0] = rookLMoved[1] = rookRMoved[0] = rookRMoved[1] = true;

    for (char ch : castling) {
        if (ch == 'K') { kingMoved[0] = false; rookRMoved[0] = false; }
        if (ch == 'Q') { kingMoved[0] = false; rookLMoved[0] = false; }
        if (ch == 'k') { kingMoved[1] = false; rookRMoved[1] = false; }
        if (ch == 'q') { kingMoved[1] = false; rookLMoved[1] = false; }
    }

    currentHash = computeHash();
}

void runBasicTest() {
    cout << "Running the ZabavaEngine test " << endl;


    string epd = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
    loadFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    cout << "Position loaded. Move " << (whiteToMove ? "Whites" : "Blacks") << endl;
    cout << "Actual Hash: " << hex << currentHash << dec << endl;


}

unsigned long long perft(int depth) {
    if (depth == 0) return 1ULL;

    unsigned long long nodes = 0;

   vector<Move> moves = generateLegalMoves();

    for (const auto& m : moves) {
        Undo u = makeMove(m);
        nodes += perft(depth - 1);
        undoMove(m, u);
    }

    return nodes;
}



void runFullTestSuite() {
    vector<PerftTest> tests = {
        {
            "Initial Position",
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            {1, 20, 400, 8902, 197281, 4865609}
        },
        {
            "Kiwipete (Position 2 - Рокировки, взятия, связки)",
            "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
            {1, 48, 2039, 97862, 4085603}
        },
        {
            "Position 3 (Эндшпиль, проходные пешки)",
            "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
            {1, 14, 191, 2812, 43238, 674624}
        },
        {
            "Position 4 (Сложные шахи и двойные шахи)",
            "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
            {1, 6, 264, 9467, 422333}
        }
    };

    for (const auto& t : tests) {
        cout << "\n--- Testing: " << t.name << " ---" << endl;
        loadFen(t.fen);
        for (int d = 1; d < t.results.size(); d++) {
            unsigned long long nodes = perft(d);
            if (nodes == t.results[d]) {
                cout << "Depth " << d << ": OK (" << nodes << ")" << endl;
            } else {
                cout << "Depth " << d << ": FAILED! Got " << nodes << " expected " << t.results[d] << endl;
                return;
            }
        }
    }
}

void perftDivide(int depth) {
    unsigned long long totalNodes = 0;
    vector<Move> moves = generateLegalMoves();

    cout << "Perft divide for depth " << depth << ":" << endl;

    for (const auto& m : moves) {
        Undo u = makeMove(m);
        unsigned long long nodes = perft(depth - 1);
        undoMove(m, u);

        totalNodes += nodes;
        cout << moveToString(m) << ": " << nodes << endl;
    }

    cout << "\nTotal nodes at depth " << depth << ": " << totalNodes << endl;
}

int main() {
    initZobrist();
    runBasicTest();
    runFullTestSuite();

    string kiwipete = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    loadFen(kiwipete);


    perftDivide(5);

    return 0;
}
