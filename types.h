#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    NullPiece = 0,
    King = 1,
    Pawn = 2,
    Knight = 3,
    Bishop = 4,
    Rook = 5,
    Queen = 6,
} PieceType;

char get_piece_type_char(PieceType type, bool is_white);

typedef struct {
    uint8_t type;
    uint8_t x;
    uint8_t y;
} Piece;

typedef enum {
    // Not a special move
    Normal = 0,

    // Pawn moves 2 spaces on its first move
    PawnDoubleMove = 1,
    // En passant
    EnPassant = 2,

    // Promote pawn
    PromoteKnight = Knight,
    PromoteBishop = Bishop,
    PromoteRook = Rook,
    PromoteQueen = Queen,

    // Castling
    KingSideCastle = 7,
    QueenSideCastle = 8,

    // Null move
    NullMove = 9,
} MoveType;

typedef struct {
    uint8_t piece_id;
    uint8_t to_x;
    uint8_t to_y;
    uint8_t special_move;
} Move;

static const Move NULL_MOVE = (Move){NullPiece, 0, 0, NullMove};

typedef struct {
    int32_t score;
    Move move;
} BestMove;

typedef struct {
    Move *moves;
    uint8_t n_moves;
} MoveList;

typedef struct {
    int64_t alpha;
    int64_t beta;
} ContextHash;

typedef struct {
    // White pieces
    Piece white_pieces[16];
    // Black pieces
    Piece black_pieces[16];
    // Castling rights
    bool white_can_castle_queen_side;
    bool white_can_castle_king_side;
    bool black_can_castle_queen_side;
    bool black_can_castle_king_side;
    // The most recent move
    Move prev_move;
    // If true, it is white's turn, otherwise it is black's turn
    bool is_white;

    // A pointer to our pieces
    Piece *our_pieces;
    // A pointer to the opponent's pieces
    Piece *opponent_pieces;
    // Bitboard for all pieces, white or black
    uint64_t piece_bb;
    // Bitboard for our pieces
    uint64_t our_bb;
    // Bitboard for opponent pieces
    uint64_t opponent_bb;

    ContextHash hash;
} PlyContext;

// Update pointers to our_pieces and opponent_pieces
#define UPDATE_POINTERS(context) { \
    context.our_pieces = context.is_white ? context.white_pieces : context.black_pieces; \
    context.opponent_pieces = context.is_white ? context.black_pieces : context.white_pieces; \
}
#endif