#include <stdlib.h>

#include "eval.h"
#include "config.h"
#include "movegen.h"
#include "precomp.h"
#include "history.h"

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

int32_t get_pawn_y_bonus(uint8_t y, bool is_white) {
    return PAWN_Y_VALUE_BONUS * (is_white ? y : 7 - y);
}

int32_t evaluate_material(PlyContext *context) {
    int32_t our_total = 0, enemy_total = 0;
    for (int i = 0; i < 16; i++) {
        our_total += get_piece_base_value(context->our_pieces[i]);
        our_total += PIECE_POSSIBLE_MOVES_BONUS_MULTIPLIER * get_piece_possible_n_moves(context->our_pieces[i], context->is_white);
        if (context->our_pieces[i].type == Pawn) {
            our_total += get_pawn_y_bonus(context->our_pieces[i].y, context->is_white);
        }

        enemy_total += get_piece_base_value(context->opponent_pieces[i]);
        enemy_total += PIECE_POSSIBLE_MOVES_BONUS_MULTIPLIER * get_piece_possible_n_moves(context->opponent_pieces[i], !context->is_white);
        if (context->opponent_pieces[i].type == Pawn) {
            enemy_total += get_pawn_y_bonus(context->opponent_pieces[i].y, !context->is_white);
        }
    }
    return (10000 * (our_total - enemy_total)) / our_total;
}

int32_t evaluate_with(PlyContext *context, MoveList legal_moves) {
    if (legal_moves.n_moves == 0) {
        return is_in_check(context) ? LOSS_VALUE : DRAW_VALUE;
    }
    return evaluate_material(context);
}

int32_t evaluate(PlyContext *context) {
    if (!has_legal_move(context)) {
        return is_in_check(context) ? LOSS_VALUE : DRAW_VALUE;
    }
    return evaluate_material(context);
}