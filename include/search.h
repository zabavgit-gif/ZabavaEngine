#ifndef SEARCH_H
#define SEARCH_H

#include "defs.h"
#include <chrono>

extern int history[2][13][64];
extern Move killerMoves[64][2];
extern std::vector<TTEntry> transpositionTable;
extern std::chrono::steady_clock::time_point searchStartTime;
extern int allocatedTime;
extern bool stopSearch;

int scoreMove(const Move& m, Move ttMove, int ply);
int quiescence(int a, int b, bool maxm, int ply);
int alphaBeta(int d, int ply, int a, int b, bool maxm, bool can_null);

#endif