#include <time.h>
#include <stdlib.h>

#include "hash.h"

const ContextHash NULL_HASH = {.alpha = 0, .beta = 0};

ContextHash rand_hash(void) {
    return (ContextHash){
        .alpha = (int64_t)rand() + ((int64_t)rand() << 32),
        .beta = (int64_t)rand() + ((int64_t)rand() << 32)
    };
}

ContextHash xor_hash(ContextHash a, ContextHash b) {
    return (ContextHash){
        .alpha = a.alpha ^ b.alpha,
        .beta = a.beta ^ b.beta
    };
}

bool is_hash_eq(ContextHash a, ContextHash b) {
    return (a.alpha == b.alpha) && (a.beta == b.beta);
}

ContextHash PAWN_FIRST_MOVE_TABLE[8];
ContextHash PIECE_HASH_TABLE[12 * 64];
ContextHash IS_WHITE_TURN_HASH;
ContextHash WHITE_CAN_CASTLE_QUEEN_SIDE_HASH;
ContextHash WHITE_CAN_CASTLE_KING_SIDE_HASH;
ContextHash BLACK_CAN_CASTLE_QUEEN_SIDE_HASH;
ContextHash BLACK_CAN_CASTLE_KING_SIDE_HASH;

ContextHash WHITE_CASTLE_QUEEN_SIDE_HASH;
ContextHash WHITE_CASTLE_KING_SIDE_HASH;
ContextHash BLACK_CASTLE_QUEEN_SIDE_HASH;
ContextHash BLACK_CASTLE_KING_SIDE_HASH;

ContextHash get_piece_hash(Piece piece, bool is_white) {
    uint8_t piece_id = (piece.type - 1) + (is_white * 6);
    return PIECE_HASH_TABLE[(piece_id << 6) + (piece.y << 3) + piece.x];
}

ContextHash get_prev_move_hash(Move prev_move) {
    return prev_move.special_move == PawnDoubleMove ?
        PAWN_FIRST_MOVE_TABLE[prev_move.to_x] : NULL_HASH;
}

ContextHash get_context_hash(PlyContext *context) {
    ContextHash hash = context->is_white ? IS_WHITE_TURN_HASH : NULL_HASH;
    UPDATE_HASH(hash, context->white_can_castle_queen_side ? WHITE_CAN_CASTLE_QUEEN_SIDE_HASH : NULL_HASH);
    UPDATE_HASH(hash, context->white_can_castle_king_side ? WHITE_CAN_CASTLE_KING_SIDE_HASH : NULL_HASH);
    UPDATE_HASH(hash, context->black_can_castle_queen_side ? BLACK_CAN_CASTLE_QUEEN_SIDE_HASH : NULL_HASH);
    UPDATE_HASH(hash, context->black_can_castle_king_side ? BLACK_CAN_CASTLE_KING_SIDE_HASH : NULL_HASH);
    for (int i = 0; i < 16; i++) {
        if (context->white_pieces[i].type != NullPiece) {
            UPDATE_HASH(hash, get_piece_hash(context->white_pieces[i], true));
        }
        if (context->black_pieces[i].type != NullPiece) {
            UPDATE_HASH(hash, get_piece_hash(context->black_pieces[i], false));
        }
    }
    UPDATE_HASH(hash, get_prev_move_hash(context->prev_move))
    return hash;
}

void init_hashing(void) {
    srand(time(NULL));
    for (int i = 0; i < 8; i++) {
        PAWN_FIRST_MOVE_TABLE[i] = rand_hash();
    }
    for (int i = 0; i < (12 * 64); i++) {
        PIECE_HASH_TABLE[i] = rand_hash();
    }
    IS_WHITE_TURN_HASH = rand_hash();
    WHITE_CAN_CASTLE_QUEEN_SIDE_HASH = rand_hash();
    WHITE_CAN_CASTLE_KING_SIDE_HASH = rand_hash();
    BLACK_CAN_CASTLE_QUEEN_SIDE_HASH = rand_hash();
    BLACK_CAN_CASTLE_KING_SIDE_HASH = rand_hash();

    WHITE_CASTLE_QUEEN_SIDE_HASH = WHITE_CAN_CASTLE_QUEEN_SIDE_HASH;
    UPDATE_HASH(WHITE_CASTLE_QUEEN_SIDE_HASH, get_piece_hash((Piece){King, 4, 0}, true))
    UPDATE_HASH(WHITE_CASTLE_QUEEN_SIDE_HASH, get_piece_hash((Piece){King, 2, 0}, true))
    UPDATE_HASH(WHITE_CASTLE_QUEEN_SIDE_HASH, get_piece_hash((Piece){Rook, 0, 0}, true))
    UPDATE_HASH(WHITE_CASTLE_QUEEN_SIDE_HASH, get_piece_hash((Piece){Rook, 3, 0}, true))

    WHITE_CASTLE_KING_SIDE_HASH = WHITE_CAN_CASTLE_KING_SIDE_HASH;
    UPDATE_HASH(WHITE_CASTLE_KING_SIDE_HASH, get_piece_hash((Piece){King, 4, 0}, true))
    UPDATE_HASH(WHITE_CASTLE_KING_SIDE_HASH, get_piece_hash((Piece){King, 6, 0}, true))
    UPDATE_HASH(WHITE_CASTLE_KING_SIDE_HASH, get_piece_hash((Piece){Rook, 7, 0}, true))
    UPDATE_HASH(WHITE_CASTLE_KING_SIDE_HASH, get_piece_hash((Piece){Rook, 5, 0}, true))

    BLACK_CASTLE_QUEEN_SIDE_HASH = BLACK_CAN_CASTLE_QUEEN_SIDE_HASH;
    UPDATE_HASH(BLACK_CASTLE_QUEEN_SIDE_HASH, get_piece_hash((Piece){King, 4, 7}, true))
    UPDATE_HASH(BLACK_CASTLE_QUEEN_SIDE_HASH, get_piece_hash((Piece){King, 2, 7}, true))
    UPDATE_HASH(BLACK_CASTLE_QUEEN_SIDE_HASH, get_piece_hash((Piece){Rook, 0, 7}, true))
    UPDATE_HASH(BLACK_CASTLE_QUEEN_SIDE_HASH, get_piece_hash((Piece){Rook, 3, 7}, true))

    BLACK_CASTLE_KING_SIDE_HASH = BLACK_CAN_CASTLE_KING_SIDE_HASH;
    UPDATE_HASH(BLACK_CASTLE_KING_SIDE_HASH, get_piece_hash((Piece){King, 4, 7}, true))
    UPDATE_HASH(BLACK_CASTLE_KING_SIDE_HASH, get_piece_hash((Piece){King, 6, 7}, true))
    UPDATE_HASH(BLACK_CASTLE_KING_SIDE_HASH, get_piece_hash((Piece){Rook, 7, 7}, true))
    UPDATE_HASH(BLACK_CASTLE_KING_SIDE_HASH, get_piece_hash((Piece){Rook, 5, 7}, true))
}