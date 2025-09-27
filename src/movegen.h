#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "types.h"

// Gets "pseudo-"legal moves for a pawn.
// That is, it does not account for pawn moves which put their own king in check.
// Does *not* ensure that the given piece is a pawn.
MoveList get_pseudo_legal_moves_pawn(PlyContext *context, uint8_t piece_id);

// Gets "pseudo-"legal moves for a knight.
// That is, it does not account for knight moves which put their own king in check.
// Does *not* ensure that the given piece is a knight.
MoveList get_pseudo_legal_moves_knight(PlyContext *context, uint8_t piece_id);

// Gets "pseudo-"legal moves for a bishop.
// That is, it does not account for bishop moves which put their own king in check.
// Does *not* ensure that the given piece is a bishop, since we reuse this for queens.
MoveList get_pseudo_legal_moves_bishop(PlyContext *context, uint8_t piece_id);

// Gets "pseudo-"legal moves for a rook.
// That is, it does not account for rook moves which put their own king in check.
// Does *not* ensure that the given piece is a rook, since we reuse this for queens.
MoveList get_pseudo_legal_moves_rook(PlyContext *context, uint8_t piece_id);

// Gets "pseudo-"legal moves for a queen.
// That is, it does not account for queen moves which put their own king in check.
// Does *not* ensure that the given piece is a queen.
MoveList get_pseudo_legal_moves_queen(PlyContext *context, uint8_t piece_id);

// Gets "pseudo-"legal moves for a king.
// That is, it does not account for king moves which put itself in check.
// Does *not* ensure that the given piece is a king.
// Also does *not* handle castling moves.
MoveList get_pseudo_legal_moves_king(PlyContext *context, uint8_t piece_id);

bool is_in_check(PlyContext *context);

uint64_t get_our_attack_bb(PlyContext *context);

uint64_t get_opponent_attack_bb(PlyContext *context);

// Adds all legal castling moves for the given color and opponent attack bitboard.
MoveList get_legal_moves_castling(PlyContext *context, uint64_t opponent_attack_bb);

MoveList get_all_legal_moves(PlyContext *context);

bool has_legal_move(PlyContext *context);

uint64_t perft(PlyContext *context, uint8_t depth);

#endif