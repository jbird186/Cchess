#ifndef CONTEXT_H
#define CONTEXT_H

#include "types.h"

// Create a new PlyContext of a board in its default state, and white to play
PlyContext new_context();

// Create a branch where some move occurs.
// Updates the context accordingly.
PlyContext create_context_branch(PlyContext context, Move move);

#endif