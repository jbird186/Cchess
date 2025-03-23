#include <stdio.h>
#include <stdlib.h>

#include "precomp.h"
#include "movegen.h"
#include "context.h"

uint64_t WHITE_PAWN_POSSIBLE_ATTACK_BB_TABLE[64];
uint64_t BLACK_PAWN_POSSIBLE_ATTACK_BB_TABLE[64];
uint64_t KING_POSSIBLE_ATTACK_BB_TABLE[64];
uint64_t KNIGHT_POSSIBLE_ATTACK_BB_TABLE[64];
uint64_t BISHOP_POSSIBLE_ATTACK_BB_TABLE[64];
uint64_t ROOK_POSSIBLE_ATTACK_BB_TABLE[64];
uint64_t QUEEN_POSSIBLE_ATTACK_BB_TABLE[64];

uint8_t WHITE_PAWN_POSSIBLE_N_MOVES_TABLE[64];
uint8_t BLACK_PAWN_POSSIBLE_N_MOVES_TABLE[64];
uint8_t KING_POSSIBLE_N_MOVES_TABLE[64];
uint8_t KNIGHT_POSSIBLE_N_MOVES_TABLE[64];
uint8_t BISHOP_POSSIBLE_N_MOVES_TABLE[64];
uint8_t ROOK_POSSIBLE_N_MOVES_TABLE[64];
uint8_t QUEEN_POSSIBLE_N_MOVES_TABLE[64];

#define INIT_PAWN_TABLE(piece_type, pseudo_legal_move_gen, attack_table_name, move_n_table_name, start_y, end_y, is_white) \
    for (int i = 0; i <= 63; i++) { \
        (attack_table_name)[i] = 0; \
        (move_n_table_name)[i] = 0; \
    } \
    for (int y = (start_y); y <= (end_y); y++) { \
        for (int x = 0; x <= 7; x++) { \
            Piece piece = {(piece_type), x, y}; \
            PlyContext context; \
            new_precomp_context(&context, piece, (is_white)); \
            \
            (attack_table_name)[(y << 3) + x] = get_our_attack_bb(&context); \
            MoveList move_list = pseudo_legal_move_gen(&context, 0); \
            free(move_list.moves); \
            \
            (move_n_table_name)[(y << 3) + x] = move_list.n_moves; \
        } \
    } \
    // for (int i = 0; i <= 63; i++) { \
    //     printf("W: %d, ID: %d, POS: %u, ATTACK MAP: %lu, N MOVES: %u\n", is_white, piece_type, i, attack_table_name[i], move_n_table_name[i]); \
    // }

#define INIT_PIECE_TABLE(piece_type, pseudo_legal_move_gen, attack_table_name, move_n_table_name) \
    INIT_PAWN_TABLE(piece_type, pseudo_legal_move_gen, attack_table_name, move_n_table_name, 0, 7, true)

void init_precomp() {
    INIT_PAWN_TABLE(Pawn, get_pseudo_legal_moves_pawn,
        WHITE_PAWN_POSSIBLE_ATTACK_BB_TABLE, WHITE_PAWN_POSSIBLE_N_MOVES_TABLE, 0, 6, true)
    INIT_PAWN_TABLE(Pawn, get_pseudo_legal_moves_pawn,
        BLACK_PAWN_POSSIBLE_ATTACK_BB_TABLE, BLACK_PAWN_POSSIBLE_N_MOVES_TABLE, 1, 7, false)
    INIT_PIECE_TABLE(King, get_pseudo_legal_moves_king,
        KING_POSSIBLE_ATTACK_BB_TABLE, KING_POSSIBLE_N_MOVES_TABLE)
    INIT_PIECE_TABLE(Knight, get_pseudo_legal_moves_knight,
        KNIGHT_POSSIBLE_ATTACK_BB_TABLE, KNIGHT_POSSIBLE_N_MOVES_TABLE)
    INIT_PIECE_TABLE(Bishop, get_pseudo_legal_moves_bishop,
        BISHOP_POSSIBLE_ATTACK_BB_TABLE, BISHOP_POSSIBLE_N_MOVES_TABLE)
    INIT_PIECE_TABLE(Rook, get_pseudo_legal_moves_rook,
        ROOK_POSSIBLE_ATTACK_BB_TABLE, ROOK_POSSIBLE_N_MOVES_TABLE)
    INIT_PIECE_TABLE(Queen, get_pseudo_legal_moves_queen,
        QUEEN_POSSIBLE_ATTACK_BB_TABLE, QUEEN_POSSIBLE_N_MOVES_TABLE)
}

uint64_t get_piece_possible_attack_bb(Piece piece, bool is_white) {
    switch(piece.type) {
        case NullPiece:
            return 0;
        case Pawn:
            if (is_white) {
                return WHITE_PAWN_POSSIBLE_ATTACK_BB_TABLE[(piece.y << 3) + piece.x];
            } else {
                return BLACK_PAWN_POSSIBLE_ATTACK_BB_TABLE[(piece.y << 3) + piece.x];
            }
        case King:
            return KING_POSSIBLE_ATTACK_BB_TABLE[(piece.y << 3) + piece.x];
        case Knight:
            return KNIGHT_POSSIBLE_ATTACK_BB_TABLE[(piece.y << 3) + piece.x];
        case Bishop:
            return BISHOP_POSSIBLE_ATTACK_BB_TABLE[(piece.y << 3) + piece.x];
        case Rook:
            return ROOK_POSSIBLE_ATTACK_BB_TABLE[(piece.y << 3) + piece.x];
        case Queen:
            return QUEEN_POSSIBLE_ATTACK_BB_TABLE[(piece.y << 3) + piece.x];
    }
}

uint8_t get_piece_possible_n_moves(Piece piece, bool is_white) {
    switch(piece.type) {
        case NullPiece:
            return 0;
        case Pawn:
            if (is_white) {
                return WHITE_PAWN_POSSIBLE_N_MOVES_TABLE[(piece.y << 3) + piece.x];
            } else {
                return BLACK_PAWN_POSSIBLE_N_MOVES_TABLE[(piece.y << 3) + piece.x];
            }
        case King:
            return KING_POSSIBLE_N_MOVES_TABLE[(piece.y << 3) + piece.x];
        case Knight:
            return KNIGHT_POSSIBLE_N_MOVES_TABLE[(piece.y << 3) + piece.x];
        case Bishop:
            return BISHOP_POSSIBLE_N_MOVES_TABLE[(piece.y << 3) + piece.x];
        case Rook:
            return ROOK_POSSIBLE_N_MOVES_TABLE[(piece.y << 3) + piece.x];
        case Queen:
            return QUEEN_POSSIBLE_N_MOVES_TABLE[(piece.y << 3) + piece.x];
    }
}