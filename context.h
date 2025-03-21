#ifndef CONTEXT_H
#define CONTEXT_H

#include "types.h"

// Create a new PlyContext of a board in its default state, and white to play
PlyContext new_context();

// Create an empty board, except for the given piece, which is assigned to white (ID = 0)
PlyContext new_precomp_context(Piece piece, bool is_white);

// Create a branch where some move occurs.
// Updates the context accordingly.
PlyContext create_context_branch(PlyContext context, Move move);

#endif