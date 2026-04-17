#include "board.h"
#include <random>

using namespace std;

Piece board[8][8];
bool whiteToMove = true;
bool kingMoved[2], rookLMoved[2], rookRMoved[2];
unsigned long long zobristTable[8][8][13];
unsigned long long zobristBlackToMove;
unsigned long long zobristCastling[16];
unsigned long long currentHash;
int enPassantCol = -1;

void initZobrist() {
    mt19937_64 rng(1234567);
    uniform_int_distribution<unsigned long long> dist;
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            for (int p = 0; p < 13; p++)
                zobristTable[r][c][p] = dist(rng);
    zobristBlackToMove = dist(rng);
    for (int i = 0; i < 16; i++)
        zobristCastling[i] = dist(rng);
}

void initBoard() {
    for(int r=0; r<8; r++)
        for(int c=0; c<8; c++)
            board[r][c] = EMPTY;

    for(int c=0; c<8; c++) {
        board[6][c] = WP;
        board[1][c] = BP;
    }


    Piece wPieces[] = { WR, WN, WB, WQ, WK, WB, WN, WR };
    Piece bPieces[] = { BR, BN, BB, BQ, BK, BB, BN, BR };

    for(int c=0; c<8; c++) {
        board[7][c] = wPieces[c];
        board[0][c] = bPieces[c];
    }

    for(int i=0; i<2; i++) kingMoved[i]=rookLMoved[i]=rookRMoved[i]=false;
    whiteToMove = true;
    initZobrist();
    currentHash = computeHash();
}

int getCastlingIndex() {
    int idx = 0;
    if (!kingMoved[0]) {
        if (!rookRMoved[0]) idx |= 1;
        if (!rookLMoved[0]) idx |= 2;
    }
    if (!kingMoved[1]) {
        if (!rookRMoved[1]) idx |= 4;
        if (!rookLMoved[1]) idx |= 8;
    }
    return idx;
}

unsigned long long computeHash() {
    unsigned long long h = 0;
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            if (board[r][c] != EMPTY)
                h ^= zobristTable[r][c][board[r][c]];
    if (!whiteToMove) h ^= zobristBlackToMove;
    h ^= zobristCastling[getCastlingIndex()];
    return h;
}

Undo makeMove(const Move& m) {
    Undo u;
    u.prevEpCol = enPassantCol;
    u.oldHash = currentHash;
    u.captured = board[m.tr][m.tc];
    u.promotedFrom = EMPTY;

    for(int i=0; i<2; i++) {
        u.kM[i]=kingMoved[i]; u.rL[i]=rookLMoved[i]; u.rR[i]=rookRMoved[i];
    }

    if (u.captured == WR) {
        if (m.tr == 7 && m.tc == 0) rookLMoved[0] = true;
        if (m.tr == 7 && m.tc == 7) rookRMoved[0] = true;
    } else if (u.captured == BR) {
        if (m.tr == 0 && m.tc == 0) rookLMoved[1] = true;
        if (m.tr == 0 && m.tc == 7) rookRMoved[1] = true;
    }

    Piece p = board[m.fr][m.fc];
    enPassantCol = -1;
    currentHash ^= zobristTable[m.fr][m.fc][p];
    if(u.captured != EMPTY) currentHash ^= zobristTable[m.tr][m.tc][u.captured];
    currentHash ^= zobristCastling[getCastlingIndex()];


if(m.flag == 1) {
    u.promotedFrom = p;
    p = (Piece)m.promotion;
}
    else if(m.flag == 2) {
        int captureRow = whiteToMove ? m.tr + 1 : m.tr - 1;
        u.captured = board[captureRow][m.tc];
        board[captureRow][m.tc] = EMPTY;
    }


    if((p == WP || p == BP) && abs(m.fr - m.tr) == 2) {
        enPassantCol = m.fc;
    }

    if((p==WK || p==BK) && abs(m.fc-m.tc)==2) {
        if(m.tc > m.fc) { board[m.tr][m.tc-1]=board[m.tr][7]; board[m.tr][7]=EMPTY; }
        else { board[m.tr][m.tc+1]=board[m.tr][0]; board[m.tr][0]=EMPTY; }
    }

    board[m.tr][m.tc] = p; board[m.fr][m.fc] = EMPTY;
    currentHash ^= zobristTable[m.tr][m.tc][p];

    if(p==WK) kingMoved[0]=true; if(p==BK) kingMoved[1]=true;
    if(p==WR) { if(m.fr==7 && m.fc==0) rookLMoved[0]=true; if(m.fr==7 && m.fc==7) rookRMoved[0]=true; }
    if(p==BR) { if(m.fr==0 && m.fc==0) rookLMoved[1]=true; if(m.fr==0 && m.fc==7) rookRMoved[1]=true; }

    whiteToMove = !whiteToMove;
    currentHash ^= zobristBlackToMove;
    currentHash ^= zobristCastling[getCastlingIndex()];
    return u;
}

void undoMove(const Move& m, const Undo& u) {
    whiteToMove = !whiteToMove;


    Piece p = board[m.tr][m.tc];

       if(u.promotedFrom != EMPTY) p = u.promotedFrom;

    board[m.fr][m.fc] = p;
    board[m.tr][m.tc] = EMPTY;


    if (m.flag == 2) {
        int captureRow = (whiteToMove) ? m.tr + 1 : m.tr - 1;
        board[captureRow][m.tc] = u.captured;
    } else {
        board[m.tr][m.tc] = u.captured;
    }


    if ((p == WK || p == BK) && abs(m.fc - m.tc) == 2) {
        if (m.tc > m.fc) {
            board[m.tr][7] = board[m.tr][m.tc - 1];
            board[m.tr][m.tc - 1] = EMPTY;
        } else {
            board[m.tr][0] = board[m.tr][m.tc + 1];
            board[m.tr][m.tc + 1] = EMPTY;
        }
    }


    enPassantCol = u.prevEpCol;
    for(int i=0; i<2; i++) {
        kingMoved[i] = u.kM[i];
        rookLMoved[i] = u.rL[i];
        rookRMoved[i] = u.rR[i];
    }
    currentHash = u.oldHash;
}