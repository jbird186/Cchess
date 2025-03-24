#include <stdio.h>

#include "game.h"
#include "types.h"

char get_piece_type_char(PieceType type, bool is_white) {
    uint8_t shift = is_white ? 0 : 32;
    switch (type) {
        case NullPiece:
            return ' ';
        case King:
            return 'K' + shift;
        case Pawn:
            return 'P' + shift;
        case Knight:
            return 'N' + shift;
        case Bishop:
            return 'B' + shift;
        case Rook:
            return 'R' + shift;
        case Queen:
            return 'Q' + shift;
    }
}

void print_board(PlyContext *context, bool display_as_white) {
    Piece white_piece, black_piece;
    char piece_char;
    int x, y;
    printf("  ---------------------------------\n");
    for (int _y = 0; _y < 8; _y++) {
        y = display_as_white ? (7 - _y) : _y;
        printf("%u ", y + 1);
        for (int _x = 0; _x < 8; _x++) {
            x = display_as_white ? _x : (7 - _x);
            piece_char = ' ';
            for (int i = 0; i < 16; i++) {
                white_piece = context->white_pieces[i];
                if ((white_piece.type != NullPiece) && (white_piece.x == x) && (white_piece.y == y)) {
                    piece_char = get_piece_type_char(white_piece.type, true);
                    break;
                }
                black_piece = context->black_pieces[i];
                if ((black_piece.type != NullPiece) && (black_piece.x == x) && (black_piece.y == y)) {
                    piece_char = get_piece_type_char(black_piece.type, false);
                    break;
                }
            }
            printf("| %c ", piece_char);
        }
        printf("|\n  ---------------------------------\n");
    }
    if (display_as_white) {
        printf("    a   b   c   d   e   f   g   h\n");
    } else {
        printf("    h   g   f   e   d   c   b   a\n");
    }
    printf("%s to play.\n", context->is_white ? "White" : "Black");
}

void print_history(GameHistory *history) {
    printf("Game history:");
    if (history->length == 0) {
        printf(" None");
    }
    for (int i = 0; i < history->length; i++) {
        printf(" %s", history->moves[i]);
    }
    printf("\n\n");
}