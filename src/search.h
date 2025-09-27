#ifndef SEARCH_H
#define SEARCH_H

#include "types.h"

typedef struct {
    ContextHash hash;
    BestMove best_move;
    uint8_t depth;

    // TODO: Re-enable
    // int32_t floor;
    // int32_t ceiling;

    // Whether or not this move is a leaf in the search tree
    bool is_leaf;
} BestMoveCacheEntry;

// const int BestMoveCacheEntrySize = sizeof(BestMoveCacheEntry);

typedef struct {
    BestMoveCacheEntry *entries;
} BestMoveCache;

BestMoveCache new_move_cache(void);

// Minimax search with alpha-beta pruning
BestMove get_best_move_ab(StateRepetitions *repetitions, PlyContext *context, int32_t depth);

#endif