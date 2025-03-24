#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>

static const int32_t N_REPETITIONS_DRAW = 3;

///// Maximum Moves /////
// These are the maximum possible moves that each piece can make in one ply.
// These values are used for allocating memory for MoveLists.

// The value for pawns is so high because of promotions.
static const int32_t PAWN_MAX_MOVES = 12;
static const int32_t KNIGHT_MAX_MOVES = 8;
static const int32_t BISHOP_MAX_MOVES = 13;
static const int32_t ROOK_MAX_MOVES = 14;
static const int32_t KING_MAX_MOVES = 8;
static const int32_t CASTLING_MAX_MOVES = 2;

///// Bitboards /////
// Note that these are mirrored across the x-axis due to the way that bitboard masks are calculated.

// The marked squares must be *empty* for castling to be legal.
static const uint64_t WHITE_QUEEN_SIDE_CASTLING_PIECES_MASK =  0b00001110;
static const uint64_t WHITE_KING_SIDE_CASTLING_PIECES_MASK =   0b01100000;
static const uint64_t BLACK_QUEEN_SIDE_CASTLING_PIECES_MASK = WHITE_QUEEN_SIDE_CASTLING_PIECES_MASK << 56;
static const uint64_t BLACK_KING_SIDE_CASTLING_PIECES_MASK = WHITE_KING_SIDE_CASTLING_PIECES_MASK << 56;

// The marked squares must *not be attacked by the enemy* for castling to be legal.
static const uint64_t WHITE_QUEEN_SIDE_CASTLING_ATTACK_MASK =  0b00011100;
static const uint64_t WHITE_KING_SIDE_CASTLING_ATTACK_MASK =   0b01110000;
static const uint64_t BLACK_QUEEN_SIDE_CASTLING_ATTACK_MASK = WHITE_QUEEN_SIDE_CASTLING_ATTACK_MASK << 56;
static const uint64_t BLACK_KING_SIDE_CASTLING_ATTACK_MASK = WHITE_KING_SIDE_CASTLING_ATTACK_MASK << 56;

// Shortcuts to flip all of the appropriate bits at once on the bitboards.
static const uint64_t WHITE_QUEEN_SIDE_CASTLING_BB_XOR =   0b00011101;
static const uint64_t WHITE_KING_SIDE_CASTLING_BB_XOR =    0b11110000;
static const uint64_t BLACK_QUEEN_SIDE_CASTLING_BB_XOR = WHITE_QUEEN_SIDE_CASTLING_BB_XOR << 56;
static const uint64_t BLACK_KING_SIDE_CASTLING_BB_XOR = WHITE_KING_SIDE_CASTLING_BB_XOR << 56;

#endif