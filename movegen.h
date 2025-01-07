#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "types.h"

bool is_in_check(PlyContext *context);

uint64_t get_opponent_attack_bb(PlyContext context);

MoveList get_all_legal_moves(PlyContext context);

uint64_t perft(PlyContext context, uint8_t depth);

#endif