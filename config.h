#ifndef CONFIG_H
#define CONFIG_H

#include "types.h"

///// Piece Values /////
static const int32_t PAWN_BASE_VALUE = 1000;
static const int32_t KNIGHT_BASE_VALUE = 3000;
static const int32_t BISHOP_BASE_VALUE = 3300;
static const int32_t ROOK_BASE_VALUE = 5000;
static const int32_t QUEEN_BASE_VALUE = 9000;

///// Game End Values /////
static const int32_t WIN_VALUE = 1000000000;
static const int32_t LOSS_VALUE = -WIN_VALUE;
static const int32_t DRAW_VALUE = 0;

///// Other Evaluation Values /////
// #define N_LEGAL_MOVES_BONUS(n_moves) ((n_moves) + 63 - (64 / (n_moves)))
#define N_LEGAL_MOVES_BONUS(n_moves) (n_moves * 10)

///// Game Configuration /////
static const bool DEFAULT_LOCK_DISPLAY = false;
static const uint8_t MOVE_SEARCH_DEPTH = 5;

#endif