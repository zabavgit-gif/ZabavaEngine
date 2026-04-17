#ifndef BOARD_H
#define BOARD_H

#include "defs.h"

extern Piece board[8][8];
extern bool whiteToMove;
extern bool kingMoved[2], rookLMoved[2], rookRMoved[2];
extern unsigned long long zobristTable[8][8][13];
extern unsigned long long zobristBlackToMove;
extern unsigned long long zobristCastling[16];
extern unsigned long long currentHash;

void initZobrist();
void initBoard();
int getCastlingIndex();
unsigned long long computeHash();
Undo makeMove(const Move& m);
void undoMove(const Move& m, const Undo& u);
extern int enPassantCol;

#endif