#ifndef CONTEXT_H
#define CONTEXT_H

#include "types.h"

// Create a new PlyContext of a board in its default state, and white to play
void new_context(PlyContext *context);

// Create an empty board, except for the given piece, which is assigned to white (ID = 0)
void new_precomp_context(PlyContext *context, Piece piece, bool is_white);

// Updates the context such that the given move is played
void update_context(PlyContext *context, Move move);

// Copy a PlyContext
void copy_context(PlyContext *from, PlyContext *to);

// Copy a PlyContext, such that the given move is played on branch
void new_context_branch(PlyContext *original, PlyContext *branch, Move move);

#endif