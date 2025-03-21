#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "context.h"
#include "game.h"
#include "types.h"
#include "movegen.h"
#include "search.h"
#include "eval.h"
#include "config.h"
#include "precomp.h"
#include "hash.h"

void init() {
    init_precomp();
    init_hashing();
}

void clear_input_buffer() {
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// TODO: Draws
int main(int argc, char **argv) {
    init();

    int arg_i = 1;
    PlyContext context = new_context();
    UPDATE_POINTERS(context)
    BestMoveCache cache = new_lookup_table();

    char (*move_hist)[6] = malloc(sizeof(char) * 6 * 1024);
    PlyContext *context_hist = malloc(sizeof(PlyContext) * 1024);
    uint32_t hist_len = 0;

    MoveList legal_moves;
    legal_moves.moves = NULL;
    char (*legal_move_codes)[6] = malloc(sizeof(char) * 6 * 1024);

    bool auto_play_white = false, auto_play_black = false;
    bool display_as_white, lock_display = DEFAULT_LOCK_DISPLAY;
    for (;;) {
        char input[256] = "";
        display_as_white = lock_display ? display_as_white : context.is_white;
        print_board(&context, display_as_white);
        // printf("HASH: %lu %lu\n", context.hash.alpha, context.hash.beta);

        free(legal_moves.moves);
        MoveList legal_moves = get_all_legal_moves(&context);
        for (int i = 0; i < legal_moves.n_moves; i++) {
            Piece piece = context.our_pieces[legal_moves.moves[i].piece_id];

            legal_move_codes[i][0] = piece.x + 'a';
            legal_move_codes[i][1] = piece.y + '1';
            legal_move_codes[i][2] = legal_moves.moves[i].to_x + 'a';
            legal_move_codes[i][3] = legal_moves.moves[i].to_y + '1';
            switch (legal_moves.moves[i].special_move) {
                case PromoteKnight:
                    legal_move_codes[i][4] = 'n';
                    legal_move_codes[i][5] = '\0';
                    break;
                case PromoteBishop:
                    legal_move_codes[i][4] = 'b';
                    legal_move_codes[i][5] = '\0';
                    break;
                case PromoteRook:
                    legal_move_codes[i][4] = 'r';
                    legal_move_codes[i][5] = '\0';
                    break;
                case PromoteQueen:
                    legal_move_codes[i][4] = 'q';
                    legal_move_codes[i][5] = '\0';
                    break;
                default:
                    legal_move_codes[i][4] = '\0';
                    break;
            }
        }

        if (legal_moves.n_moves == 0) {
            if (is_in_check(&context)) {
                printf("Checkmate. %s wins.\n", context.is_white ? "Black" : "White");
            } else {
                printf("Stalemate.\n");
            }
            break;
        }

        bool should_auto_play = context.is_white ? auto_play_white : auto_play_black;

        // Set the input variable for this loop iteration, if applicable
        if (!should_auto_play) {
            // If there are more external commands to read, read them
            if (arg_i < argc) {
                strcpy(input, argv[arg_i++]);
            // If there are no more external commands to read, prompt the user
            } else {
                printf("Enter a move or command: ");
                if (!fgets(input, sizeof(input), stdin)) {
                    printf("Failed to read user input.\n");
                    break;
                }
                input[strcspn(input, "\n")] = 0;
            }
        }

        // Exit the program
        if (strcmp(input, "exit") == 0) {
            break;
        }

        // Undo the previous move(s)
        if (strncmp(input, "back", 4) == 0) {
            uint32_t depth;

            // If no depth is specified, default to 1
            if (strcmp(input, "back") == 0) {
                depth = 1;
            // Attempt to read a valid depth
            } else if ((sscanf(input + 4, "%u", &depth) != 1) || (depth <= 0)) {
                printf("Invalid depth.\n\n");
                continue;
            }

            if (depth > hist_len) {
                printf("Depth (%u ply) exceeds history depth (%u ply).\n\n", depth, hist_len);
                continue;
            }

            hist_len -= depth;
            context = context_hist[hist_len];
            UPDATE_POINTERS(context)
            printf("%u move%s sucessfully undone:", depth, depth == 1 ? "" : "s");
            for (int i = hist_len + depth; i > hist_len; i--) {
                printf(" %s", move_hist[i - 1]);
            }
            printf("\n\n");
            continue;
        }

        // Lock the display to its current orientation
        if (strcmp(input, "lock") == 0) {
            lock_display = true;
            printf("Locked board orientation to %s's perspective.\n\n", display_as_white ? "White" : "Black");
            continue;
        }

        // Unlock the display, causing it to change orientation for each player
        if (strcmp(input, "unlock") == 0) {
            lock_display = false;
            printf("Unlocked board orientation.\n\n");
            continue;
        }

        // Display history
        if (strcmp(input, "hist") == 0) {
            if (hist_len == 0) {
                printf("No history to display.\n\n");
                continue;
            }
            for (int i = 0; i < hist_len; i++) {
                printf("%s ", move_hist[i]);
            }
            printf("\n\n");
            continue;
        }

        // List all legal moves
        if (strcmp(input, "list") == 0) {
            for (int i = 0; i < legal_moves.n_moves; i++) {
                printf("%s ", legal_move_codes[i]);
            }
            printf("\n\n");
            continue;
        }

        // Run perft
        if (strncmp(input, "perft", 5) == 0) {
            uint8_t depth;
            uint64_t nodes = 0;
            if (sscanf(input + 5, "%hhu", &depth) == 1) {
                printf("Running perft at depth %d...\n", depth);
                if (depth <= 0) {
                    printf("Found 1 total node.\n\n");
                    continue;
                }

                PlyContext branch;
                uint64_t branch_nodes;
                for (int i = 0; i < legal_moves.n_moves; i++) {
                    branch = create_context_branch(context, legal_moves.moves[i]);
                    UPDATE_POINTERS(branch)
                    branch_nodes = perft(&branch, depth - 1);
                    printf("\t%s: %lu nodes\n", legal_move_codes[i], branch_nodes);
                    nodes += branch_nodes;
                }
                printf("Found %lu total nodes.\n\n", nodes);

            } else {
                printf("Invalid depth.\n\n");
            }
            continue;
        }

        // From now on, auto-play as this color
        if (strcmp(input, "auto") == 0) {
            if (context.is_white) {
                auto_play_white = true;
                printf("The computer will automatically select moves for white from now on.\n");
            } else {
                auto_play_black = true;
                printf("The computer will automatically select moves for black from now on.\n");
            }
            printf("\n");
            continue;
        }

        // Computer should play this move
        if ((strcmp(input, "play") == 0) || should_auto_play) {
            printf("Computer is thinking...");
            fflush(stdout);
            BestMove best_move = get_best_move_ab(&context, &cache, MOVE_SEARCH_DEPTH);

            Piece piece = context.our_pieces[best_move.move.piece_id];

            move_hist[hist_len][0] = piece.x + 'a';
            move_hist[hist_len][1] = piece.y + '1';
            move_hist[hist_len][2] = best_move.move.to_x + 'a';
            move_hist[hist_len][3] = best_move.move.to_y + '1';
            switch (best_move.move.special_move) {
                case PromoteKnight:
                    move_hist[hist_len][4] = 'n';
                    move_hist[hist_len][5] = '\0';
                    break;
                case PromoteBishop:
                    move_hist[hist_len][4] = 'b';
                    move_hist[hist_len][5] = '\0';
                    break;
                case PromoteRook:
                    move_hist[hist_len][4] = 'r';
                    move_hist[hist_len][5] = '\0';
                    break;
                case PromoteQueen:
                    move_hist[hist_len][4] = 'q';
                    move_hist[hist_len][5] = '\0';
                    break;
                default:
                    move_hist[hist_len][4] = '\0';
                    break;
            }

            context_hist[hist_len] = context;
            printf(" %s\n\n", move_hist[hist_len]);
            hist_len++;

            context = create_context_branch(context, best_move.move);
            UPDATE_POINTERS(context)
            continue;
        }

        // User should play this move
        bool found_move = false;
        for (int i = 0; i < legal_moves.n_moves; i++) {
            if (strcmp(legal_move_codes[i], input) == 0) {
                found_move = true;

                context_hist[hist_len] = context;
                strcpy(move_hist[hist_len++], input);

                context = create_context_branch(context, legal_moves.moves[i]);
                UPDATE_POINTERS(context)
                printf("\n");
                break;
            }
        }
        if (!found_move) {
            printf("Please enter a valid move or command.\n\n");
        }
    }
    printf("Game history:");
    for (int i = 0; i < hist_len; i++) {
        printf(" %s", move_hist[i]);
    }
    printf("\n\n");

    free(cache.entries);
    free(context_hist);
    free(move_hist);
    free(legal_moves.moves);
    free(legal_move_codes);
    return 0;
}