#include "constants.h"
#include "context.h"
#include "position.h"

// Create a new PlyContext of a board in its default state, and white to play
PlyContext new_context() {
    PlyContext context = {
        .white_pieces = {
            (Piece){Rook, 0, 0, false},
            (Piece){Knight, 1, 0, false},
            (Piece){Bishop, 2, 0, false},
            (Piece){Queen, 3, 0, false},
            (Piece){King, 4, 0, false},
            (Piece){Bishop, 5, 0, false},
            (Piece){Knight, 6, 0, false},
            (Piece){Rook, 7, 0, false},
            (Piece){Pawn, 0, 1, false},
            (Piece){Pawn, 1, 1, false},
            (Piece){Pawn, 2, 1, false},
            (Piece){Pawn, 3, 1, false},
            (Piece){Pawn, 4, 1, false},
            (Piece){Pawn, 5, 1, false},
            (Piece){Pawn, 6, 1, false},
            (Piece){Pawn, 7, 1, false},
        },
        .black_pieces = {
            (Piece){Rook, 0, 7, false},
            (Piece){Knight, 1, 7, false},
            (Piece){Bishop, 2, 7, false},
            (Piece){Queen, 3, 7, false},
            (Piece){King, 4, 7, false},
            (Piece){Bishop, 5, 7, false},
            (Piece){Knight, 6, 7, false},
            (Piece){Rook, 7, 7, false},
            (Piece){Pawn, 0, 6, false},
            (Piece){Pawn, 1, 6, false},
            (Piece){Pawn, 2, 6, false},
            (Piece){Pawn, 3, 6, false},
            (Piece){Pawn, 4, 6, false},
            (Piece){Pawn, 5, 6, false},
            (Piece){Pawn, 6, 6, false},
            (Piece){Pawn, 7, 6, false},
        },
        .prev_move = (Move){0, 0, 0, Normal},
        .is_white = true,
    };

    UPDATE_POINTERS(context)

    context.our_bb = 0;
    context.opponent_bb = 0;
    for (int i = 0; i < 16; i++) {
        if (context.our_pieces[i].type != NullPiece)
            context.our_bb |= GET_PIECE_BB_MASK(context.our_pieces[i]);
        if (context.opponent_pieces[i].type != NullPiece)
            context.opponent_bb |= GET_PIECE_BB_MASK(context.opponent_pieces[i]);
    }
    context.piece_bb = context.our_bb | context.opponent_bb;
    return context;
}

// Create a branch where some move occurs.
// Updates the context accordingly.
PlyContext create_context_branch(PlyContext context, Move move) {
    UPDATE_POINTERS(context)
    context.prev_move = move;
    context.is_white = !context.is_white;

    // The perspective is flipped so we don't have to update the pointers later
    context.our_pieces = context.is_white ? context.white_pieces : context.black_pieces;
    context.opponent_pieces = context.is_white ? context.black_pieces : context.white_pieces;

    uint64_t tmp = context.our_bb;
    context.our_bb = context.opponent_bb;
    context.opponent_bb = tmp;

    // Handle special moves
    switch (move.special_move) {
        case EnPassant: {
            uint8_t pawn_piece_id = 8 + move.to_x;
            context.our_bb ^= GET_PIECE_BB_MASK(context.our_pieces[pawn_piece_id]);
            context.our_pieces[pawn_piece_id].type = NullPiece;
            break;
        }

        case PromoteKnight:
            context.opponent_pieces[move.piece_id].type = Knight;
            break;
        case PromoteBishop:
            context.opponent_pieces[move.piece_id].type = Bishop;
            break;
        case PromoteRook:
            context.opponent_pieces[move.piece_id].type = Rook;
            break;
        case PromoteQueen:
            context.opponent_pieces[move.piece_id].type = Queen;
            break;

        case KingSideCastle:
            // Move the king
            context.opponent_pieces[4].has_moved = true;
            context.opponent_pieces[4].x = 6;
            // Move the rook
            context.opponent_pieces[7].has_moved = true;
            context.opponent_pieces[7].x = 5;
            // Update bitboards
            context.opponent_bb ^= context.is_white ?
                BLACK_KING_SIDE_CASTLING_BB_XOR : WHITE_KING_SIDE_CASTLING_BB_XOR;
            context.piece_bb = context.our_bb | context.opponent_bb;
            // We don't want to update any positions at the end
            return context;

        case QueenSideCastle:
            // Move the king
            context.opponent_pieces[4].has_moved = true;
            context.opponent_pieces[4].x = 2;
            // Move the rook
            context.opponent_pieces[0].has_moved = true;
            context.opponent_pieces[0].x = 3;
            // Update bitboards
            context.opponent_bb ^= context.is_white ?
                BLACK_QUEEN_SIDE_CASTLING_BB_XOR : WHITE_QUEEN_SIDE_CASTLING_BB_XOR;
            context.piece_bb = context.our_bb | context.opponent_bb;
            // We don't want to update any positions at the end
            return context;
        case NullMove:
            return context;
    }

    // Acknowledge that this piece has moved
    context.opponent_pieces[move.piece_id].has_moved = true;
    context.opponent_bb ^= GET_PIECE_BB_MASK(context.opponent_pieces[move.piece_id]);

    // Update this piece's position
    context.opponent_pieces[move.piece_id].x = move.to_x;
    context.opponent_pieces[move.piece_id].y = move.to_y;
    uint64_t piece_mask = GET_PIECE_BB_MASK(context.opponent_pieces[move.piece_id]);
    context.opponent_bb |= piece_mask;

    // Remove an opponent piece, if necessary
    if ((piece_mask & context.our_bb) != 0) {
        for (int i = 0; i < 16; i++) {
            // If already removed, skip
            if (context.our_pieces[i].type == NullPiece)
                continue;

            // Search for the piece with matching coordinates
            if ((context.our_pieces[i].x == move.to_x) && (context.our_pieces[i].y == move.to_y)) {
                context.our_pieces[i].type = NullPiece;
                context.our_bb ^= piece_mask;
                break;
            }
        }
    }
    context.piece_bb = context.our_bb | context.opponent_bb;
    return context;
}