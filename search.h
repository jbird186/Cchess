#ifndef SEARCH_H
#define SEARCH_H

#include "types.h"

// Minimax search with alpha-beta pruning
BestMove get_best_move_ab(PlyContext *context, int32_t depth);

#endif