#ifndef HISTORY_H
#define HISTORY_H

#include "types.h"

void free_state_repetitions(StateRepetitions *repetitions);
void _state_repetition_branch(StateRepetitions *original, StateRepetitions *branch, ContextHash hash);
uint8_t n_state_repetitions(StateRepetitions *repetitions, ContextHash hash);
bool is_draw(StateRepetitions *repetitions, ContextHash hash);
bool will_be_draw(StateRepetitions *repetitions, ContextHash hash);

void new_history(GameHistory *history);
void free_history(GameHistory *history);
void append_history(GameHistory *history, PlyContext *context, char move[6]);
void pop_history(GameHistory *history, PlyContext *context, char move[6]);
void clear_history(GameHistory *history);

#endif