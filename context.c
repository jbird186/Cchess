#include "constants.h"
#include "context.h"
#include "position.h"
#include "hash.h"

// Create a new PlyContext of a board in its default state, and white to play
PlyContext new_context() {
    PlyContext context = {
        .white_pieces = {
            (Piece){Rook, 0, 0},
            (Piece){Knight, 1, 0},
            (Piece){Bishop, 2, 0},
            (Piece){Queen, 3, 0},
            (Piece){King, 4, 0},
            (Piece){Bishop, 5, 0},
            (Piece){Knight, 6, 0},
            (Piece){Rook, 7, 0},
            (Piece){Pawn, 0, 1},
            (Piece){Pawn, 1, 1},
            (Piece){Pawn, 2, 1},
            (Piece){Pawn, 3, 1},
            (Piece){Pawn, 4, 1},
            (Piece){Pawn, 5, 1},
            (Piece){Pawn, 6, 1},
            (Piece){Pawn, 7, 1},
        },
        .black_pieces = {
            (Piece){Rook, 0, 7},
            (Piece){Knight, 1, 7},
            (Piece){Bishop, 2, 7},
            (Piece){Queen, 3, 7},
            (Piece){King, 4, 7},
            (Piece){Bishop, 5, 7},
            (Piece){Knight, 6, 7},
            (Piece){Rook, 7, 7},
            (Piece){Pawn, 0, 6},
            (Piece){Pawn, 1, 6},
            (Piece){Pawn, 2, 6},
            (Piece){Pawn, 3, 6},
            (Piece){Pawn, 4, 6},
            (Piece){Pawn, 5, 6},
            (Piece){Pawn, 6, 6},
            (Piece){Pawn, 7, 6},
        },
        .white_can_castle_queen_side = true,
        .white_can_castle_king_side = true,
        .black_can_castle_queen_side = true,
        .black_can_castle_king_side = true,
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

    context.hash = get_context_hash(&context);
    return context;
}

PlyContext new_precomp_context(Piece piece, bool is_white) {
    PlyContext context = new_context();
    if (!is_white) {
        context = create_context_branch(context, NULL_MOVE);
    }
    UPDATE_POINTERS(context)

    for (int i = 0; i < 16; i++) {
        context.white_pieces[i].type = NullPiece;
        context.black_pieces[i].type = NullPiece;
    }
    context.our_pieces[0] = piece;
    context.our_bb = GET_PIECE_BB_MASK(piece);
    context.opponent_bb = 0;
    context.piece_bb = context.our_bb;
    return context;
}

void remove_white_king_side_castling_rights(PlyContext *context) {
    if (context->white_can_castle_king_side) {
        UPDATE_HASH(context->hash, WHITE_CAN_CASTLE_KING_SIDE_HASH);
    }
    context->white_can_castle_king_side = false;
}
void remove_white_queen_side_castling_rights(PlyContext *context) {
    if (context->white_can_castle_queen_side) {
        UPDATE_HASH(context->hash, WHITE_CAN_CASTLE_QUEEN_SIDE_HASH);
    }
    context->white_can_castle_queen_side = false;
}
void remove_black_king_side_castling_rights(PlyContext *context) {
    if (context->black_can_castle_king_side) {
        UPDATE_HASH(context->hash, BLACK_CAN_CASTLE_KING_SIDE_HASH);
    }
    context->black_can_castle_king_side = false;
}
void remove_black_queen_side_castling_rights(PlyContext *context) {
    if (context->black_can_castle_queen_side) {
        UPDATE_HASH(context->hash, BLACK_CAN_CASTLE_QUEEN_SIDE_HASH);
    }
    context->black_can_castle_queen_side = false;
}

// Create a branch where some move occurs.
// Updates the context accordingly.
PlyContext create_context_branch(PlyContext context, Move move) {
    UPDATE_HASH(context.hash, get_prev_move_hash(context.prev_move))
    UPDATE_HASH(context.hash, get_prev_move_hash(move))
    context.prev_move = move;
    UPDATE_HASH(context.hash, IS_WHITE_TURN_HASH)
    context.is_white = !context.is_white;

    // The perspective is flipped so we don't have to update the pointers later
    UPDATE_POINTERS(context)

    uint64_t tmp = context.our_bb;
    context.our_bb = context.opponent_bb;
    context.opponent_bb = tmp;

    bool has_promoted = false;
    // Handle special moves
    switch (move.special_move) {
        case EnPassant: {
            uint8_t pawn_piece_id = 8 + move.to_x;
            UPDATE_HASH(context.hash, get_piece_hash(context.our_pieces[pawn_piece_id], context.is_white));
            context.our_bb ^= GET_PIECE_BB_MASK(context.our_pieces[pawn_piece_id]);
            context.our_pieces[pawn_piece_id].type = NullPiece;
            break;
        }

        case PromoteKnight:
            context.opponent_pieces[move.piece_id].type = Knight;
            has_promoted = true;
            break;
        case PromoteBishop:
            context.opponent_pieces[move.piece_id].type = Bishop;
            has_promoted = true;
            break;
        case PromoteRook:
            context.opponent_pieces[move.piece_id].type = Rook;
            has_promoted = true;
            break;
        case PromoteQueen:
            context.opponent_pieces[move.piece_id].type = Queen;
            has_promoted = true;
            break;

        case KingSideCastle:
            // Remove castling rights. Recall that the perspective is flipped.
            if (context.is_white) {
                context.black_can_castle_king_side = false;
                UPDATE_HASH(context.hash, BLACK_CASTLE_KING_SIDE_HASH)
            } else {
                context.white_can_castle_king_side = false;
                UPDATE_HASH(context.hash, WHITE_CASTLE_KING_SIDE_HASH)
            }
            // Move the king
            context.opponent_pieces[4].x = 6;
            // Move the rook
            context.opponent_pieces[7].x = 5;
            // Update bitboards
            context.opponent_bb ^= context.is_white ?
                BLACK_KING_SIDE_CASTLING_BB_XOR : WHITE_KING_SIDE_CASTLING_BB_XOR;
            context.piece_bb = context.our_bb | context.opponent_bb;
            // We don't want to update any positions at the end
            return context;

        case QueenSideCastle:
            // Remove castling rights. Recall that the perspective is flipped.
            if (context.is_white) {
                context.black_can_castle_queen_side = false;
                UPDATE_HASH(context.hash, BLACK_CASTLE_QUEEN_SIDE_HASH)
            } else {
                context.white_can_castle_queen_side = false;
                UPDATE_HASH(context.hash, WHITE_CASTLE_QUEEN_SIDE_HASH)
            }
            // Move the king
            context.opponent_pieces[4].x = 2;
            // Move the rook
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

    // Check if the King has moved, to remove castling rights
    if (move.piece_id == 4) {
        if (context.is_white) {
            remove_black_king_side_castling_rights(&context);
            remove_black_queen_side_castling_rights(&context);
        } else {
            remove_white_king_side_castling_rights(&context);
            remove_white_queen_side_castling_rights(&context);
        }
    } else if (move.piece_id == 7) {
        if (context.is_white) {
            remove_black_king_side_castling_rights(&context);
        } else {
            remove_white_king_side_castling_rights(&context);
        }
    } else if (move.piece_id == 0) {
        if (context.is_white) {
            remove_black_queen_side_castling_rights(&context);
        } else {
            remove_white_queen_side_castling_rights(&context);
        }
    }
    context.opponent_bb ^= GET_PIECE_BB_MASK(context.opponent_pieces[move.piece_id]);

    // Update this piece's position
    Piece to_remove_from_hash;
    if (has_promoted) {
        to_remove_from_hash = (Piece){
            .type = Pawn,
            .x = context.opponent_pieces[move.piece_id].x,
            .y = context.opponent_pieces[move.piece_id].y
        };
    } else {
        to_remove_from_hash = context.opponent_pieces[move.piece_id];
    }
    UPDATE_HASH(context.hash, get_piece_hash(to_remove_from_hash, !context.is_white))
    context.opponent_pieces[move.piece_id].x = move.to_x;
    context.opponent_pieces[move.piece_id].y = move.to_y;
    UPDATE_HASH(context.hash, get_piece_hash(context.opponent_pieces[move.piece_id], !context.is_white))
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
                UPDATE_HASH(context.hash, get_piece_hash(context.our_pieces[i], context.is_white));
                context.our_pieces[i].type = NullPiece;
                context.our_bb ^= piece_mask;
                break;
            }
        }
    }
    context.piece_bb = context.our_bb | context.opponent_bb;
    return context;
}