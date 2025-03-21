#include <stdlib.h>

#include "search.h"
#include "movegen.h"
#include "types.h"
#include "context.h"
#include "eval.h"
#include "config.h"
#include "hash.h"

#include <stdio.h>

const uint64_t LOOKUP_TABLE_SIZE = LOOKUP_TABLE_SIZE_BYTES / sizeof(BestMoveCacheEntry);

uint64_t get_table_index(ContextHash hash) {
    return (hash.alpha % LOOKUP_TABLE_SIZE);
}

BestMoveCache new_lookup_table() {
    BestMoveCacheEntry *entries = malloc(sizeof(BestMoveCacheEntry) * LOOKUP_TABLE_SIZE);
    return (BestMoveCache){entries};
}

#define UPDATE_CACHE(_is_leaf) \
    if ((cached.depth < depth) || !is_hash_eq(context->hash, cached.hash)) { \
        cache->entries[get_table_index(context->hash)] = (BestMoveCacheEntry){ \
            .hash = context->hash, \
            .best_move = (best_move), \
            .depth = depth, \
            .is_leaf = _is_leaf \
        }; \
    }

// Minimax search with alpha-beta pruning
BestMove _get_best_move_ab(PlyContext *context, BestMoveCache *cache, int32_t depth, int32_t floor, int32_t ceiling) {
    // Check if the cache contains this state
    BestMoveCacheEntry cached = cache->entries[get_table_index(context->hash)];
    // TODO: Allow non-leaf results to be returned
    if ((cached.depth >= depth) && is_hash_eq(context->hash, cached.hash)) {
        return cached.best_move;
    }

    if (depth == 0) {
        BestMove best_move = {evaluate(context), NULL_MOVE};
        UPDATE_CACHE(true)
        return best_move;
    }

    MoveList legal_moves = get_all_legal_moves(context);
    if (legal_moves.n_moves == 0) {
        free(legal_moves.moves);
        BestMove best_move = {evaluate_with(context, legal_moves), NULL_MOVE};
        UPDATE_CACHE(true)
        return best_move;
    }

    Move move = NULL_MOVE;
    int32_t score = LOSS_VALUE * 2;
    PlyContext branch;
    for (int i = 0; i < legal_moves.n_moves; i++) {
        new_context_branch(context, &branch, legal_moves.moves[i]);

        BestMove opponent_best = _get_best_move_ab(&branch, cache, depth - 1, -ceiling, -floor);
        int32_t branch_score = -opponent_best.score;

        if (branch_score > score) {
            score = branch_score;
            move = legal_moves.moves[i];
        }

        // TODO: account for floor/ceiling in cache
        if (branch_score > ceiling)
            ceiling = branch_score;
        if (branch_score >= floor)
            break;
    }

    free(legal_moves.moves);
    BestMove best_move = {score, move};
    // TODO: Re-enable cache for non-leaf results.
    // For some reason, these yield bad results?
    // UPDATE_CACHE(false)
    return best_move;
}

BestMove get_best_move_ab(PlyContext *context, BestMoveCache *cache, int32_t depth) {
    return _get_best_move_ab(context, cache, depth, 1000000000, -1000000000);
}