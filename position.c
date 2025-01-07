#include "position.h"

uint64_t get_move_list_bb_mask(MoveList move_list) {
    uint64_t result = 0;
    for (int i = 0; i < move_list.n_moves; i++) {
        result |= GET_MOVE_BB_MASK(move_list.moves[i]);
    }
    return result;
}