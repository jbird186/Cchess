#ifndef POSITION_H
#define POSITION_H

#include "types.h"

#define GET_POS_BB_MASK(pos) ((uint64_t)1 << (pos))

#define GET_PIECE_POS(piece) ((piece.y << 3) + piece.x)

#define GET_PIECE_BB_MASK(piece) GET_POS_BB_MASK(GET_PIECE_POS(piece))

#define GET_MOVE_POS(move) ((move.to_y << 3) + move.to_x)

#define GET_MOVE_BB_MASK(move) GET_POS_BB_MASK(GET_MOVE_POS(move))

#endif