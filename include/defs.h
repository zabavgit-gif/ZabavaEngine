#ifndef DEFS_H
#define DEFS_H
#include <vector>
#include <string>

enum Piece { EMPTY, WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK };
enum TTFlag { EXACT, ALPHA, BETA };

struct Move {
    int fr, fc, tr, tc, score;
    int flag;
    int promotion;
    bool operator==(const Move& other) const {
        return fr == other.fr && fc == other.fc &&
            tr == other.tr && tc == other.tc &&
               promotion == other.promotion;
    }
    Move(int f_r, int f_c, int t_r, int t_c, int f = 0, int p = EMPTY)
        : fr(f_r), fc(f_c), tr(t_r), tc(t_c), flag(f), promotion(p) {}

    Move() : fr(0), fc(0), tr(0), tc(0), flag(0), promotion(EMPTY) {}

};

struct Undo {
    Piece captured;
    Piece promotedFrom;
    bool kM[2], rL[2], rR[2];
    unsigned long long oldHash;
    int prevEpCol;
};

struct TTEntry {
    unsigned long long key;
    int depth;
    int score;
    TTFlag flag;
    Move bestMove;
};

const int MAX_PLY = 64;
const int MG = 0;
const int EG = 1;

extern int pieceValues[13][2];
extern int mvv_lva_values[];


extern const int pawnTable[64];
extern const int knightTable[64];
extern const int bishopTable[64];
extern const int rookTable[64];
extern const int queenTable[64];
extern const int kingTable[64];

#endif