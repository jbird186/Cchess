#ifndef CONFIG_H
#define CONFIG_H

#include "types.h"

///// Piece Values /////
#define PAWN_BASE_VALUE 1000
#define KNIGHT_BASE_VALUE 3000
#define BISHOP_BASE_VALUE 3300
#define ROOK_BASE_VALUE 5000
#define QUEEN_BASE_VALUE 9000

///// Game End Values /////
#define WIN_VALUE 1000000000
#define LOSS_VALUE -1000000000
#define DRAW_VALUE 0

///// Other Evaluation Values /////
// #define N_LEGAL_MOVES_BONUS(n_moves) ((n_moves) + 63 - (64 / (n_moves)))
#define PIECE_POSSIBLE_MOVES_BONUS_MULTIPLIER 20
#define PAWN_Y_VALUE_BONUS 30

///// Game Configuration /////
#define DEFAULT_LOCK_DISPLAY false
#define MOVE_SEARCH_DEPTH 4
#define LOOKUP_TABLE_SIZE_BYTES (uint64_t)(512 * 1024 * 1024)

#endif