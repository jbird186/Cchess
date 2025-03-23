#include <stdlib.h>

#include "constants.h"
#include "context.h"
#include "movegen.h"
#include "position.h"
#include "precomp.h"

#define APPEND_PROMOTION_MOVES(piece_id, to_x, to_y) { \
    moves[n_moves++] = (Move){(piece_id), (to_x), (to_y), PromoteKnight}; \
    moves[n_moves++] = (Move){(piece_id), (to_x), (to_y), PromoteBishop}; \
    moves[n_moves++] = (Move){(piece_id), (to_x), (to_y), PromoteRook}; \
    moves[n_moves++] = (Move){(piece_id), (to_x), (to_y), PromoteQueen}; \
}

// Gets "pseudo-"legal moves for a pawn.
// That is, it does not account for pawn moves which put their own king in check.
// Does *not* ensure that the given piece is a pawn.
MoveList get_pseudo_legal_moves_pawn(PlyContext *context, uint8_t piece_id) {
    uint8_t n_moves = 0;
    Move *moves = malloc(sizeof(Move) * PAWN_MAX_MOVES);
    Piece piece = context->our_pieces[piece_id];

    int8_t forward_one_y = context->is_white ? 1 : -1;
    int8_t forward_pos = context->is_white ? 8 : -8;
    uint8_t forward_y = piece.y + forward_one_y;
    bool can_promote = (context->is_white ? 7 : 0) == forward_y;

    ///// Move Forward
    uint8_t forward_one_pos = GET_PIECE_POS(piece) + forward_pos;
    // Ensure that the space is empty
    if ((context->piece_bb & GET_POS_BB_MASK(forward_one_pos)) == 0) {
        if (can_promote) {
            // Promotion
            APPEND_PROMOTION_MOVES(piece_id, piece.x, forward_y)
        } else {
            // Move 1 forward
            moves[n_moves++] = (Move){piece_id, piece.x, forward_y, Normal};

            // Move 2 forward
            uint8_t forward_two_pos = forward_one_pos + forward_pos;
            uint8_t start_rank = context->is_white ? 1 : 6;
            // Ensure that the pawn hasn't moved yet, and the space is empty
            if ((piece.y == start_rank) && ((context->piece_bb & GET_POS_BB_MASK(forward_two_pos)) == 0)) {
                moves[n_moves++] = (Move){piece_id, piece.x, forward_y + forward_one_y, PawnDoubleMove};
            };
        }
    };

    bool can_en_passant =
        (context->prev_move.special_move == PawnDoubleMove) &&
        (piece.y == context->prev_move.to_y);

    ///// Attack left
    // Ensure that the pawn isn't on the left edge
    if (piece.x != 0) {
        uint8_t left_x = piece.x - 1;
        uint64_t attack_left_pos = forward_one_pos - 1;

        ///// Regular attack
        // Ensure that the space has an enemy piece
        if ((context->opponent_bb & GET_POS_BB_MASK(attack_left_pos)) != 0) {
            if (can_promote) {
                // Attack and promote
                APPEND_PROMOTION_MOVES(piece_id, left_x, forward_y)
            } else {
                // Normal attack
                moves[n_moves++] = (Move){piece_id, left_x, forward_y, Normal};
            }
        }
        ///// En passant
        else if (can_en_passant && (left_x == context->prev_move.to_x)) {
            moves[n_moves++] = (Move){piece_id, left_x, forward_y, EnPassant};
        }
    }

    ///// Attack right
    // Ensure that the pawn isn't on the right edge
    if (piece.x != 7) {
        uint8_t right_x = piece.x + 1;
        uint64_t attack_right_pos = forward_one_pos + 1;

        ///// Regular attack
        // Ensure that the space has an enemy piece
        if ((context->opponent_bb & GET_POS_BB_MASK(attack_right_pos)) != 0) {
            if (can_promote) {
                // Attack and promote
                APPEND_PROMOTION_MOVES(piece_id, right_x, forward_y)
            } else {
                // Normal attack
                moves[n_moves++] = (Move){piece_id, right_x, forward_y, Normal};
            }
        }
        ///// En passant
        else if (can_en_passant && (right_x == context->prev_move.to_x)) {
            moves[n_moves++] = (Move){piece_id, right_x, forward_y, EnPassant};
        }
    }

    return (MoveList){moves, n_moves};
}

// Gets "pseudo-"legal moves for a knight.
// That is, it does not account for knight moves which put their own king in check.
// Does *not* ensure that the given piece is a knight.
MoveList get_pseudo_legal_moves_knight(PlyContext *context, uint8_t piece_id) {
    uint8_t n_moves = 0;
    Move *moves = malloc(sizeof(Move) * KNIGHT_MAX_MOVES);
    Piece piece = context->our_pieces[piece_id];

    // Possible knight offsets.
    // (We're abusing unsigned integer overflow for efficiency. 255 = -1 and 254 = -2.)
    uint8_t x_offsets[8] = {255, 1, 254, 2, 254, 2, 255, 1};
    uint8_t y_offsets[8] = {2, 2, 1, 1, 255, 255, 254, 254};

    for (int i = 0; i < 8; i++) {
        uint8_t to_x = piece.x + x_offsets[i];
        uint8_t to_y = piece.y + y_offsets[i];
        // Ensure that the target position is valid.
        // Since we're using unsigned integers, negative target coordinates will overflow to >7.
        if ((to_x > 7) || (to_y > 7))
            continue;

        // Add move if the target is *not* occupied by our pieces
        if ((context->our_bb & GET_POS_BB_MASK((to_y << 3) + to_x)) == 0)
            moves[n_moves++] = (Move){piece_id, to_x, to_y, Normal};
    }

    return (MoveList){moves, n_moves};
}

// Gets "pseudo-"legal moves for a bishop.
// That is, it does not account for bishop moves which put their own king in check.
// Does *not* ensure that the given piece is a bishop, since we reuse this for queens.
MoveList get_pseudo_legal_moves_bishop(PlyContext *context, uint8_t piece_id) {
    uint8_t n_moves = 0;
    Move *moves = malloc(sizeof(Move) * BISHOP_MAX_MOVES);
    Piece piece = context->our_pieces[piece_id];

    // (We're abusing unsigned integer overflow for efficiency. 255 = -1.)
    uint8_t offsets[4][2] = {{1, 1}, {1, 255}, {255, 1}, {255, 255}};
    uint8_t to_x;
    uint8_t to_y;
    for (int i = 0; i < 4; i++) {
        to_x = piece.x + offsets[i][0];
        to_y = piece.y + offsets[i][1];
        while ((to_x < 8) && (to_y < 8)) {
            uint8_t to_pos = (to_y << 3) + to_x;

            // Add move if the target is *not* occupied by our pieces
            if ((context->our_bb & GET_POS_BB_MASK(to_pos)) == 0)
                moves[n_moves++] = (Move){piece_id, to_x, to_y, Normal};
            // Stop searching this path once the target *is* occupied by any piece
            if ((context->piece_bb & GET_POS_BB_MASK(to_pos)) != 0)
                break;

            to_x += offsets[i][0];
            to_y += offsets[i][1];
        }
    }

    return (MoveList){moves, n_moves};
}

// Gets "pseudo-"legal moves for a rook.
// That is, it does not account for rook moves which put their own king in check.
// Does *not* ensure that the given piece is a rook, since we reuse this for queens.
MoveList get_pseudo_legal_moves_rook(PlyContext *context, uint8_t piece_id) {
    uint8_t n_moves = 0;
    Move *moves = malloc(sizeof(Move) * ROOK_MAX_MOVES);
    Piece piece = context->our_pieces[piece_id];

    // (We're abusing unsigned integer overflow for efficiency. 255 = -1.)
    uint8_t offsets[2] = {1, 255};

    // Horizontal moves
    uint8_t to_x;
    for (int i = 0; i < 2; i++) {
        to_x = piece.x + offsets[i];
        uint8_t to_pos_y = piece.y << 3;
        while (to_x < 8) {
            uint8_t to_pos = to_pos_y + to_x;

            // Add move if the target is *not* occupied by our pieces
            if ((context->our_bb & GET_POS_BB_MASK(to_pos)) == 0)
                moves[n_moves++] = (Move){piece_id, to_x, piece.y, Normal};
            // Stop searching this path once the target *is* occupied by any piece
            if ((context->piece_bb & GET_POS_BB_MASK(to_pos)) != 0)
                break;

            to_x += offsets[i];
        }
    }

    // Vertical moves
    uint8_t to_y;
    for (int i = 0; i < 2; i++) {
        to_y = piece.y + offsets[i];
        while (to_y < 8) {
            uint8_t to_pos = (to_y << 3) + piece.x;

            // Add move if the target is *not* occupied by our pieces
            if ((context->our_bb & GET_POS_BB_MASK(to_pos)) == 0)
                moves[n_moves++] = (Move){piece_id, piece.x, to_y, Normal};
            // Stop searching this path once the target *is* occupied by any piece
            if ((context->piece_bb & GET_POS_BB_MASK(to_pos)) != 0)
                break;

            to_y += offsets[i];
        }
    }

    return (MoveList){moves, n_moves};
}

// Gets "pseudo-"legal moves for a queen.
// That is, it does not account for queen moves which put their own king in check.
// Does *not* ensure that the given piece is a queen.
MoveList get_pseudo_legal_moves_queen(PlyContext *context, uint8_t piece_id) {
    Piece piece = context->our_pieces[piece_id];
    MoveList rook_moves = get_pseudo_legal_moves_rook(context, piece_id);
    MoveList bishop_moves = get_pseudo_legal_moves_bishop(context, piece_id);

    uint8_t n_moves = 0;
    Move *moves = malloc(sizeof(Move) * (rook_moves.n_moves + bishop_moves.n_moves));

    for (int i = 0; i < rook_moves.n_moves; i++) {
        moves[n_moves++] = rook_moves.moves[i];
    }
    free(rook_moves.moves);

    for (int i = 0; i < bishop_moves.n_moves; i++) {
        moves[n_moves++] = bishop_moves.moves[i];
    }
    free(bishop_moves.moves);

    return (MoveList){moves, n_moves};
}

// Gets "pseudo-"legal moves for a king.
// That is, it does not account for king moves which put itself in check.
// Does *not* ensure that the given piece is a king.
// Also does *not* handle castling moves.
MoveList get_pseudo_legal_moves_king(PlyContext *context, uint8_t piece_id) {
    uint8_t n_moves = 0;
    Move *moves = malloc(sizeof(Move) * KING_MAX_MOVES);
    Piece piece = context->our_pieces[piece_id];

    // Possible king offsets.
    // (We're abusing unsigned integer overflow for efficiency. 255 = -1.)
    uint8_t x_offsets[8] = {255, 0, 1, 255, 1, 255, 0, 1};
    uint8_t y_offsets[8] = {1, 1, 1, 0, 0, 255, 255, 255};

    for (int i = 0; i < 8; i++) {
        uint8_t to_x = piece.x + x_offsets[i];
        uint8_t to_y = piece.y + y_offsets[i];
        // Ensure that the target position is valid.
        // Since we're using unsigned integers, negative target coordinates will overflow to >7.
        if ((to_x > 7) || (to_y > 7))
            continue;

        uint8_t to_pos = (to_y << 3) + to_x;
        // Add move if the target is *not* occupied by our pieces
        if ((context->our_bb & GET_POS_BB_MASK(to_pos)) == 0)
            moves[n_moves++] = (Move){piece_id, to_x, to_y, Normal};
    }

    return (MoveList){moves, n_moves};
}

#define GET_PSEUDO_LEGAL_MOVES_GENERIC(move_list, type, context, i) \
    switch (type) { \
        case King: \
            (move_list) = get_pseudo_legal_moves_king(context, i); \
            break; \
        case Pawn: \
            (move_list) = get_pseudo_legal_moves_pawn(context, i); \
            break; \
        case Knight: \
            (move_list) = get_pseudo_legal_moves_knight(context, i); \
            break; \
        case Bishop: \
            (move_list) = get_pseudo_legal_moves_bishop(context, i); \
            break; \
        case Rook: \
            (move_list) = get_pseudo_legal_moves_rook(context, i); \
            break; \
        case Queen: \
            (move_list) = get_pseudo_legal_moves_queen(context, i); \
            break; \
        default: \
            continue; \
    }

// Checks whether a game state is legal.
// This is used to separate the actually-legal moves from the pseudo-legal-but-not-actually-legal moves.
bool is_legal_state(PlyContext *context) {
    uint64_t king_bb_mask = GET_PIECE_BB_MASK(context->opponent_pieces[4]);
    uint8_t forward_pos;
    MoveList move_list;
    Piece piece;
    for (int i = 0; i < 16; i++) {
        piece = context->our_pieces[i];
        if ((get_piece_possible_attack_bb(piece, context->is_white) & king_bb_mask) == 0) {
            continue;
        }

        // We have to handle pawn attacks manually,
        // since they will not be detected if the space they are attacking is empty.
        if (piece.type == Pawn) {
            forward_pos = GET_PIECE_POS(piece) + (context->is_white ? 8 : -8);
            if (piece.x != 0) {
                if ((GET_POS_BB_MASK(forward_pos - 1) & king_bb_mask) != 0) {
                    return false;
                };
            }
            if (piece.x != 7) {
                if ((GET_POS_BB_MASK(forward_pos + 1) & king_bb_mask) != 0) {
                    return false;
                }
            }
            continue;
        }

        GET_PSEUDO_LEGAL_MOVES_GENERIC(move_list, piece.type, context, i)
        for (int j = 0; j < move_list.n_moves; j++) {
            // Need to check special case of straight pawn movements, which are *not* attacks.
            if ((piece.type != Pawn) || (piece.x != move_list.moves[j].to_x)) {
                if ((GET_MOVE_BB_MASK(move_list.moves[j]) & king_bb_mask) != 0) {
                    free(move_list.moves);
                    return false;
                }
            }
        }
        // Free the move list for each piece
        free(move_list.moves);
    }
    return true;
}

bool is_in_check(PlyContext *context) {
    // Flip the perspective of the PlyContext from white to black or vice versa
    PlyContext opponent_context;
    new_context_branch(context, &opponent_context, NULL_MOVE);
    return !is_legal_state(&opponent_context);
}

// Returns a new MoveList with only legal moves in the given context.
// Does *not* free the given move_list's memory
MoveList filter_legal_pseudo_moves(PlyContext *context, MoveList move_list) {
    uint8_t n_moves = 0;
    Move *moves = malloc(sizeof(Move) * move_list.n_moves);
    PlyContext branch;

    for (int i = 0; i < move_list.n_moves; i++) {
        new_context_branch(context, &branch, move_list.moves[i]);
        if (is_legal_state(&branch)) {
            moves[n_moves++] = move_list.moves[i];
        }
    }

    return (MoveList){moves, n_moves};
}

uint64_t get_our_attack_bb(PlyContext *context) {
    uint8_t forward_pos;
    uint64_t result = 0;
    MoveList move_list;
    Piece piece;
    for (int i = 0; i < 16; i++) {
        piece = context->our_pieces[i];

        // We have to handle pawn attacks manually,
        // since they will not be detected if the space they are attacking is empty.
        if (piece.type == Pawn) {
            forward_pos = GET_PIECE_POS(piece) + (context->is_white ? 8 : -8);
            if (piece.x != 0) {
                result |= GET_POS_BB_MASK(forward_pos - 1);
            }
            if (piece.x != 7) {
                result |= GET_POS_BB_MASK(forward_pos + 1);
            }
            continue;
        }

        GET_PSEUDO_LEGAL_MOVES_GENERIC(move_list, piece.type, context, i)
        for (int j = 0; j < move_list.n_moves; j++) {
            // Need to check special case of straight pawn movements, which are *not* attacks.
            if ((piece.type != Pawn) || (piece.x != move_list.moves[j].to_x)) {
                result |= GET_MOVE_BB_MASK(move_list.moves[j]);
            }
        }
        // Free the move list for each piece
        free(move_list.moves);
    }
    return result;
}

uint64_t get_opponent_attack_bb(PlyContext *context) {
    // Flip the perspective of the PlyContext from white to black or vice versa
    PlyContext branch;
    new_context_branch(context, &branch, NULL_MOVE);
    return get_our_attack_bb(&branch);
}

// Adds all legal castling moves for the given color and opponent attack bitboard.
MoveList get_legal_moves_castling(PlyContext *context, uint64_t opponent_attack_bb) {
    uint8_t n_moves = 0;
    Move *moves = malloc(sizeof(Move) * CASTLING_MAX_MOVES);

    uint64_t queen_side_castling_pieces_mask = context->is_white ?
        WHITE_QUEEN_SIDE_CASTLING_PIECES_MASK : BLACK_QUEEN_SIDE_CASTLING_PIECES_MASK;
    uint64_t king_side_castling_pieces_mask = context->is_white ?
        WHITE_KING_SIDE_CASTLING_PIECES_MASK : BLACK_KING_SIDE_CASTLING_PIECES_MASK;
    uint64_t queen_side_castling_attack_mask = context->is_white ?
        WHITE_QUEEN_SIDE_CASTLING_ATTACK_MASK : BLACK_QUEEN_SIDE_CASTLING_ATTACK_MASK;
    uint64_t king_side_castling_attack_mask = context->is_white ?
        WHITE_KING_SIDE_CASTLING_ATTACK_MASK : BLACK_KING_SIDE_CASTLING_ATTACK_MASK;
    uint8_t back_rank_index = context->is_white ? 0 : 7;

    // Queen side castling
    if ((context->is_white ? context->white_can_castle_queen_side : context->black_can_castle_queen_side)
        && ((context->piece_bb & queen_side_castling_pieces_mask) == 0)
        && ((opponent_attack_bb & queen_side_castling_attack_mask) == 0)
    ) {
        moves[n_moves++] = (Move){4, 2, back_rank_index, QueenSideCastle};
    }

    // King side castling
    if ((context->is_white ? context->white_can_castle_king_side : context->black_can_castle_king_side)
        && ((context->piece_bb & king_side_castling_pieces_mask) == 0)
        && ((opponent_attack_bb & king_side_castling_attack_mask) == 0)
    ) {
        moves[n_moves++] = (Move){4, 6, back_rank_index, KingSideCastle};
    }

    return (MoveList){moves, n_moves};
}

MoveList get_all_legal_moves(PlyContext *context) {
    uint8_t total_n_moves = 0;
    Move *total_moves;
    uint8_t n_move_lists = 0;
    MoveList move_lists[17];

    MoveList raw_piece_moves, piece_moves;
    for (int i = 0; i < 16; i++) {
        GET_PSEUDO_LEGAL_MOVES_GENERIC(raw_piece_moves, context->our_pieces[i].type, context, i)
        // We will free this later, when collecting the legal moves from all pieces
        piece_moves = filter_legal_pseudo_moves(context, raw_piece_moves);
        // Free the unfiltered move list for each piece
        free(raw_piece_moves.moves);

        total_n_moves += piece_moves.n_moves;
        move_lists[n_move_lists++] = piece_moves;
    }

    uint64_t opponent_attack_bb = get_opponent_attack_bb(context);
    // We will free this later, when collecting the legal moves from all pieces
    piece_moves = get_legal_moves_castling(context, opponent_attack_bb);
    total_n_moves += piece_moves.n_moves;
    move_lists[n_move_lists++] = piece_moves;

    // Add each piece's legal moves to the total legal moves
    total_moves = malloc(sizeof(Move) * total_n_moves);
    total_n_moves = 0;
    for (int i = 0; i < n_move_lists; i++) {
        for (int j = 0; j < move_lists[i].n_moves; j++) {
            total_moves[total_n_moves++] = move_lists[i].moves[j];
        }
        // Free the filtered move list for each piece
        free(move_lists[i].moves);
    }

    return (MoveList){total_moves, total_n_moves};
}

bool has_legal_move(PlyContext *context) {
    MoveList raw_piece_moves, piece_moves;
    for (int i = 0; i < 16; i++) {
        GET_PSEUDO_LEGAL_MOVES_GENERIC(raw_piece_moves, context->our_pieces[i].type, context, i)
        piece_moves = filter_legal_pseudo_moves(context, raw_piece_moves);
        free(raw_piece_moves.moves);
        free(piece_moves.moves);
        if (piece_moves.n_moves !=0 ) {
            return true;
        }
    }
    piece_moves = get_legal_moves_castling(context, get_opponent_attack_bb(context));
    free(piece_moves.moves);
    return (piece_moves.n_moves != 0);
}

uint64_t perft(PlyContext *context, uint8_t depth) {
    if (depth == 0) {
        return 1;
    }

    MoveList legal_moves = get_all_legal_moves(context);
    PlyContext branch;
    uint64_t total = 0;
    for (int i = 0; i < legal_moves.n_moves; i++) {
        PlyContext branch;
        new_context_branch(context, &branch, legal_moves.moves[i]);
        total += perft(&branch, depth - 1);
    }
    free(legal_moves.moves);
    return total;
}