#include "search.h"
#include "board.h"
#include "movegen.h"
#include "eval.h"
#include <algorithm>
#include <climits>

using namespace std;

int history[2][13][64];
Move killerMoves[64][2];
vector<TTEntry> transpositionTable(1 << 20);
chrono::steady_clock::time_point searchStartTime;
int allocatedTime = 1000;
bool stopSearch = false;

int scoreMove(const Move& m, Move ttMove, int ply) {
    if (m == ttMove) return 1000000;
    Piece capt = board[m.tr][m.tc];
    if (capt != EMPTY) return 900000 + (pieceValues[capt][MG] * 10 - pieceValues[board[m.fr][m.fc]][MG]);
    if (m == killerMoves[ply][0]) return 800000;
    if (m == killerMoves[ply][1]) return 700000;
    return history[whiteToMove ? 0 : 1][board[m.fr][m.fc]][m.tr * 8 + m.tc];
}

int quiescence(int a, int b, int ply) {

    int stand_pat = evaluate();
    if (stand_pat >= b) return b;
    if (stand_pat > a) a = stand_pat;

    vector<Move> captures = generateLegalCaptures();

    for (auto& mv : captures) {
        mv.score = scoreMove(mv, {-1,-1,-1,-1,0}, ply);
    }
    sort(captures.begin(), captures.end(), [](const Move& x, const Move& y) {
        return x.score > y.score;
    });


    for (auto& mv : captures) {
        Undo u = makeMove(mv);

        int score = -quiescence(-b, -a, ply + 1);

        undoMove(mv, u);

        if (score >= b) return b;
        if (score > a) a = score;
    }

    return a;
}

int alphaBeta(int d, int ply, int a, int b, bool maxm, bool can_null) {
    if (stopSearch) return 0;
    TTEntry &e = transpositionTable[currentHash % transpositionTable.size()];
    if (e.key == currentHash && e.depth >= d) {
        if (e.flag == EXACT) return e.score;
        if (e.flag == ALPHA && e.score <= a) return a;
        if (e.flag == BETA && e.score >= b) return b;
    }
    if (d <= 0) return quiescence(a, b,  ply);

    if (can_null && d >= 3 && !inCheck(whiteToMove)) {
        whiteToMove = !whiteToMove;
        currentHash ^= zobristBlackToMove;
        int score = -alphaBeta(d - 1 - 2, ply + 1, -b, -b + 1, !maxm, false);
        currentHash ^= zobristBlackToMove;
        whiteToMove = !whiteToMove;
        if (score >= b) return b;
    }

    vector<Move> ms = generateLegalMoves();
    if (ms.empty()) return inCheck(whiteToMove) ? -100000 + ply : 0;

    Move bestTTMove = (e.key == currentHash) ? e.bestMove : Move{-1,-1,-1,-1,0};
    for (auto& mv : ms) mv.score = scoreMove(mv, bestTTMove, ply);
    sort(ms.begin(), ms.end(), [](const Move& x, const Move& y) { return x.score > y.score; });

    int movesTried = 0;
    int bestScore = -INT_MAX;
    Move bestM = ms[0];

    for (auto& mv : ms) {
        Undo u = makeMove(mv);
        int score;
        if (movesTried >= 4 && d >= 3 && board[mv.tr][mv.tc] == EMPTY && !inCheck(whiteToMove)) 
            score = -alphaBeta(d - 2, ply + 1, -a - 1, -a, !maxm, true);
        else score = a + 1;

        if (score > a) score = -alphaBeta(d - 1, ply + 1, -b, -a, !maxm, true);
        undoMove(mv, u);
        movesTried++;
        if (score > bestScore) { bestScore = score; bestM = mv; }
        if (score > a) a = score;
        if (a >= b) {
            if (board[mv.tr][mv.tc] == EMPTY) {
                if (!(mv == killerMoves[ply][0])) { killerMoves[ply][1] = killerMoves[ply][0]; killerMoves[ply][0] = mv; }
                history[whiteToMove ? 0 : 1][board[mv.fr][mv.fc]][mv.tr * 8 + mv.tc] += d * d;
            }
            e = {currentHash, d, b, BETA, mv};
            return b;
        }
    }
    e = {currentHash, d, bestScore, (bestScore <= a) ? ALPHA : EXACT, bestM};
    return bestScore;
}