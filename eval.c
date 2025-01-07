#include <stdlib.h>

#include "eval.h"
#include "config.h"
#include "movegen.h"

int32_t get_piece_base_value(Piece piece) {
    switch (piece.type) {
        case Pawn:
            return PAWN_BASE_VALUE;
        case Knight:
            return KNIGHT_BASE_VALUE;
        case Bishop:
            return BISHOP_BASE_VALUE;
        case Rook:
            return ROOK_BASE_VALUE;
        case Queen:
            return QUEEN_BASE_VALUE;
        default:
            return 0;
    }
}

int32_t evaluate_with(PlyContext *context, MoveList legal_moves) {
    if (legal_moves.n_moves == 0) {
        return is_in_check(context) ? LOSS_VALUE : DRAW_VALUE;
    }

    int32_t total = N_LEGAL_MOVES_BONUS(legal_moves.n_moves);
    for (int i = 0; i < 16; i++) {
        total += get_piece_base_value(context->our_pieces[i]);
        total -= get_piece_base_value(context->opponent_pieces[i]);
    }
    return total;
}

int32_t evaluate(PlyContext *context) {
    MoveList legal_moves = get_all_legal_moves(*(context));
    int32_t score = evaluate_with(context, legal_moves);
    free(legal_moves.moves);
    return score;
}