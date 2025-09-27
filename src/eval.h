#ifndef EVAL_H
#define EVAL_H

#include "types.h"

int32_t evaluate_with(PlyContext *context, MoveList legal_moves);

int32_t evaluate(PlyContext *context);

#endif