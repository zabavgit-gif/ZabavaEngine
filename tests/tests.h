#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "board.h"
#include <vector>


std::vector<Move> generateMoves();
std::vector<Move> generateLegalMoves();
bool inCheck(bool white);
bool squareAttacked(int r, int c, bool byWhite);

#endif