#ifndef HASH_H
#define HASH_H

#include <stdbool.h>

#include "types.h"

extern ContextHash IS_WHITE_TURN_HASH;
extern ContextHash WHITE_CAN_CASTLE_QUEEN_SIDE_HASH;
extern ContextHash WHITE_CAN_CASTLE_KING_SIDE_HASH;
extern ContextHash BLACK_CAN_CASTLE_QUEEN_SIDE_HASH;
extern ContextHash BLACK_CAN_CASTLE_KING_SIDE_HASH;

extern ContextHash WHITE_CASTLE_QUEEN_SIDE_HASH;
extern ContextHash WHITE_CASTLE_KING_SIDE_HASH;
extern ContextHash BLACK_CASTLE_QUEEN_SIDE_HASH;
extern ContextHash BLACK_CASTLE_KING_SIDE_HASH;

ContextHash xor_hash(ContextHash a, ContextHash b);
#define UPDATE_HASH(to_set, b) (to_set) = xor_hash((to_set), (b));
bool is_hash_eq(ContextHash a, ContextHash b);

ContextHash get_piece_hash(Piece piece, bool is_white);
ContextHash get_prev_move_hash(Move prev_move);
ContextHash get_context_hash(PlyContext *context);

void init_hashing();

#endif