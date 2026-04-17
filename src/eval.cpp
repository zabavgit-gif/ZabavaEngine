#include "eval.h"
#include "board.h"
#include "movegen.h"

using namespace std;
int pieceValues[13][2] = {
    {0, 0}, {100, 120}, {320, 330}, {330, 340}, {500, 550}, {900, 1000}, {20000, 20000},
    {100, 120}, {320, 330}, {330, 340}, {500, 550}, {900, 1000}, {20000, 20000}
};

int mvv_lva_values[] = { 0, 100, 320, 330, 500, 900, 20000, 100, 320, 330, 500, 900, 20000 };


const int pawnTable[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

const int knightTable[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};
const int bishopTable[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

const int rookTable[64] = {
     0,  0,  0,  5,  5,  0,  0,  0,
     5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     0,  0,  0,  5,  5,  0,  0,  0
};

const int queenTable[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};
const int kingTable[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

int get_pawn_shield_penalty(int r, int c, bool white) {
    int penalty = 0;
    int dir = white ? -1 : 1;
    int pawn = white ? WP : BP;
    if (!inBounds(r + dir, c)) return 0;
    for (int df = -1; df <= 1; df++) {
        int nf = c + df;
        if (nf >= 0 && nf < 8) {
            if (board[r + dir][nf] != pawn) penalty += 30;
        }
    }
    return penalty;
}

int evaluate() {
    int mg[2] = {0, 0};
    int eg[2] = {0, 0};
    int phase = 0;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Piece p = board[r][c];
            if (p == EMPTY) continue;
            bool white = isWhite(p);
            int side = white ? 0 : 1;

            mg[side] += pieceValues[p][MG];
            eg[side] += pieceValues[p][EG];

            if (p != WP && p != BP && p != WK && p != BK) {
                if (p == WQ || p == BQ) phase += 4;
                else if (p == WR || p == BR) phase += 2;
                else phase += 1;
            }

            int idx = r * 8 + c;
            int flipIdx = white ? idx : (7 - r) * 8 + c;

            switch (p) {
                case WP: mg[0] += pawnTable[flipIdx]; break;
                case BP: mg[1] += pawnTable[flipIdx]; break;
                case WN: mg[0] += knightTable[flipIdx]; break;
                case BN: mg[1] += knightTable[flipIdx]; break;
                case WB: mg[0] += bishopTable[flipIdx]; break;
                case BB: mg[1] += bishopTable[flipIdx]; break;
                case WR: mg[0] += rookTable[flipIdx]; break;
                case BR: mg[1] += rookTable[flipIdx]; break;
                case WQ: mg[0] += queenTable[flipIdx]; break;
                case BQ: mg[1] += queenTable[flipIdx]; break;
                case WK: mg[0] += kingTable[flipIdx]; break;
                case BK: mg[1] += kingTable[flipIdx]; break;
            }

            if (p == WK && r == 7) mg[0] -= get_pawn_shield_penalty(r, c, true);
            if (p == BK && r == 0) mg[1] -= get_pawn_shield_penalty(r, c, false);
        }
    }
    int mgScore = mg[0] - mg[1];
    int egScore = eg[0] - eg[1];
    int p = (phase * 256 + (24 / 2)) / 24;
    if (p > 256) p = 256;
    int score = ((mgScore * p) + (egScore * (256 - p))) / 256;

    return whiteToMove ? score : -score;
}