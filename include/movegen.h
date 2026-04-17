#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "defs.h"
#include "board.h"
#include <vector>

bool isWhite(Piece p);
bool inBounds(int r, int c);
bool squareAttacked(int r, int c, bool byWhite);
bool inCheck(bool white);
std::vector<Move> generateMoves();
std::vector<Move> generateLegalMoves();
std::vector<Move> generateCaptures();
std::vector<Move> generateLegalCaptures();

#endif