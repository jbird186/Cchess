#include <stdlib.h>

#include "search.h"
#include "movegen.h"
#include "types.h"
#include "context.h"
#include "eval.h"
#include "config.h"

// Minimax search with alpha-beta pruning
BestMove _get_best_move_ab(PlyContext *context, int32_t depth, int32_t floor, int32_t ceiling) {
    if (depth == 0) {
        return (BestMove){evaluate(context), NULL_MOVE};
    }

    MoveList legal_moves = get_all_legal_moves(*(context));
    if (legal_moves.n_moves == 0) {
        BestMove best_move = (BestMove){evaluate_with(context, legal_moves), NULL_MOVE};
        free(legal_moves.moves);
        return best_move;
    }

    Move best_move = NULL_MOVE;
    int32_t score = LOSS_VALUE * 2;
    PlyContext branch;
    for (int i = 0; i < legal_moves.n_moves; i++) {
        branch = create_context_branch(*(context), legal_moves.moves[i]);
        UPDATE_POINTERS(branch)

        BestMove opponent_best = _get_best_move_ab(&branch, depth - 1, -ceiling, -floor);
        int32_t branch_score = -opponent_best.score;

        if (branch_score > 0) {
            branch_score--;
        } else if (branch_score < 0) {
            branch_score++;
        }

        if (branch_score > score) {
            score = branch_score;
            best_move = legal_moves.moves[i];
        }

        if (branch_score > ceiling)
            ceiling = branch_score;
        if (branch_score >= floor)
            break;
    }
    free(legal_moves.moves);
    return (BestMove){score, best_move};
}

BestMove get_best_move_ab(PlyContext *context, int32_t depth) {
    return _get_best_move_ab(context, depth, 1000000000, -1000000000);
}