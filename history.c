#include <stdlib.h>
#include <string.h>

#include "history.h"
#include "config.h"
#include "constants.h"
#include "hash.h"
#include "context.h"

void new_state_repetitions(StateRepetitions *repetition) {
    repetition->hashes = calloc(MAX_GAME_PLY, sizeof(ContextHash));
    repetition->entries = calloc(MAX_GAME_PLY, sizeof(uint8_t));
    repetition->n_entries = 0;
}

void free_state_repetitions(StateRepetitions *repetitions) {
    free(repetitions->hashes);
    free(repetitions->entries);
}

void append_state_repetition(StateRepetitions *repetition, ContextHash hash) {
    for (int i = 0; i < repetition->n_entries; i++) {
        if (is_hash_eq(repetition->hashes[i], hash)) {
            repetition->entries[i]++;
            return;
        }
    }
    repetition->hashes[repetition->n_entries] = hash;
    repetition->entries[repetition->n_entries] = 1;
    repetition->n_entries++;
}

void remove_state_repetition(StateRepetitions *repetition, ContextHash hash) {
    for (int i = 0; i < repetition->n_entries; i++) {
        if (is_hash_eq(repetition->hashes[i], hash)) {
            repetition->entries[i]--;
            if (repetition->entries[i] == 0) {
                repetition->n_entries--;
                repetition->hashes[i] = repetition->hashes[repetition->n_entries];
                repetition->entries[i] = repetition->entries[repetition->n_entries];
            }
            return;
        }
    }
}

void _state_repetition_branch(StateRepetitions *original, StateRepetitions *branch, ContextHash hash) {
    new_state_repetitions(branch);
    memcpy(branch->hashes, original->hashes, MAX_GAME_PLY * sizeof(ContextHash));
    memcpy(branch->entries, original->entries, MAX_GAME_PLY * sizeof(uint8_t));
    branch->n_entries = original->n_entries;
    append_state_repetition(branch, hash);
}

uint8_t n_state_repetitions(StateRepetitions *repetitions, ContextHash hash) {
    for (int i = 0; i < repetitions->n_entries; i++) {
        if (is_hash_eq(repetitions->hashes[i], hash)) {
            return repetitions->entries[i];
        }
    }
    return 0;
}

// TOOD: More robust draw detection
bool is_draw(StateRepetitions *repetitions, ContextHash hash) {
    return n_state_repetitions(repetitions, hash) >= N_REPETITIONS_DRAW;
}

bool will_be_draw(StateRepetitions *repetitions, ContextHash hash) {
    return n_state_repetitions(repetitions, hash) >= N_REPETITIONS_DRAW - 1;
}


void new_history(GameHistory *history) {
    history->contexts = calloc(MAX_GAME_PLY, sizeof(PlyContext));
    history->moves = calloc(MAX_GAME_PLY, sizeof(char) * 6);
    history->length = 0;
    new_state_repetitions(&history->repetitions);
}

void free_history(GameHistory *history) {
    free(history->contexts);
    free(history->moves);
    free_state_repetitions(&history->repetitions);
}

void append_history(GameHistory *history, PlyContext *context, char move[6]) {
    copy_context(context, &history->contexts[history->length]);
    strcpy(history->moves[history->length], move);
    history->length++;
    append_state_repetition(&history->repetitions, context->hash);
}

void pop_history(GameHistory *history, PlyContext *context, char move[6]) {
    history->length--;
    copy_context(&history->contexts[history->length], context);
    strcpy(move, history->moves[history->length]);
    remove_state_repetition(&history->repetitions, context->hash);
}

void clear_history(GameHistory *history) {
    history->length = 0;
}