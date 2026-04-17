#ifndef UCI_H
#define UCI_H

#include "defs.h"

void runUciMode();
void runConsoleGame();
std::string moveToString(Move m);
void printBoard();

#endif