#ifndef PRECOMP_H
#define PRECOMP_H

#include <stdint.h>

#include "types.h"

extern uint64_t WHITE_PAWN_POSSIBLE_ATTACK_BB_TABLE[64];
extern uint64_t BLACK_PAWN_POSSIBLE_ATTACK_BB_TABLE[64];
extern uint64_t KING_POSSIBLE_ATTACK_BB_TABLE[64];
extern uint64_t KNIGHT_POSSIBLE_ATTACK_BB_TABLE[64];
extern uint64_t BISHOP_POSSIBLE_ATTACK_BB_TABLE[64];
extern uint64_t ROOK_POSSIBLE_ATTACK_BB_TABLE[64];
extern uint64_t QUEEN_POSSIBLE_ATTACK_BB_TABLE[64];

extern uint8_t WHITE_PAWN_POSSIBLE_N_MOVES_TABLE[64];
extern uint8_t BLACK_PAWN_POSSIBLE_N_MOVES_TABLE[64];
extern uint8_t KING_POSSIBLE_N_MOVES_TABLE[64];
extern uint8_t KNIGHT_POSSIBLE_N_MOVES_TABLE[64];
extern uint8_t BISHOP_POSSIBLE_N_MOVES_TABLE[64];
extern uint8_t ROOK_POSSIBLE_N_MOVES_TABLE[64];
extern uint8_t QUEEN_POSSIBLE_N_MOVES_TABLE[64];

void init_precomp(void);

uint64_t get_piece_possible_attack_bb(Piece piece, bool is_white);

uint8_t get_piece_possible_n_moves(Piece piece, bool is_white);

#endif