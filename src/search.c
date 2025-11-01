#include <stdlib.h>

#include "search.h"
#include "movegen.h"
#include "types.h"
#include "context.h"
#include "eval.h"
#include "config.h"
#include "hash.h"
#include "history.h"
#include "position.h"

const uint64_t MOVE_CACHE_SIZE = MOVE_CACHE_SIZE_BYTES / sizeof(BestMoveCacheEntry);

uint64_t get_table_index(ContextHash hash) {
    return (hash.alpha % MOVE_CACHE_SIZE);
}

BestMoveCache new_move_cache(void) {
    BestMoveCacheEntry *entries = calloc(MOVE_CACHE_SIZE, sizeof(BestMoveCacheEntry));
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
BestMove _get_best_move_ab(StateRepetitions *repetitions, PlyContext *context, int32_t depth, BestMoveCache *cache, int32_t floor, int32_t ceiling) {
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
        StateRepetitions reps_branch;
        new_context_branch(context, &branch, legal_moves.moves[i]);
        _state_repetition_branch(repetitions, &reps_branch, branch.hash);

        int32_t branch_score;
        if (is_repetition_draw(&reps_branch, branch.hash)) {
            branch_score = DRAW_VALUE;
        } else {
            int32_t new_depth = (
                (depth == 1) && (GET_MOVE_BB_MASK(legal_moves.moves[i]) & branch.our_bb)
            ) ? 1 : depth - 1;

            BestMove opponent_best = _get_best_move_ab(&reps_branch, &branch, new_depth, cache, -ceiling, -floor);
            branch_score = -opponent_best.score;
            branch_score += branch_score > 0 ? -1: 1;
        }
        free_state_repetitions(&reps_branch);

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

BestMove get_best_move_ab(StateRepetitions *repetitions, PlyContext *context, int32_t depth) {
    BestMoveCache cache = new_move_cache();
    BestMove result = _get_best_move_ab(repetitions, context, depth, &cache, 1000000000, -1000000000);
    free(cache.entries);
    return result;
}