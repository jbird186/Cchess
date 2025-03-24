#ifndef GAME_H
#define GAME_H

#include "types.h"

void print_board(PlyContext *context, bool display_as_white);

void print_history(GameHistory *history);

#endif