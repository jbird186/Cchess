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
#include "history.h"

void init(void) {
    init_precomp();
    init_hashing();
}

void clear_input_buffer(void) {
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main(int argc, char **argv) {
    init();

    int arg_i = 1;

    PlyContext context;
    new_context(&context);
    GameHistory history;
    new_history(&history);

    MoveList legal_moves = { .moves = NULL, .n_moves = 0 };
    char (*legal_move_codes)[6] = malloc(sizeof(char) * 6 * MAX_GAME_PLY);

    bool auto_play_white = false, auto_play_black = false;
    bool lock_display = DEFAULT_LOCK_DISPLAY;
    bool display_as_white = true;
    for (;;) {
        char input[256] = "";
        display_as_white = lock_display ? display_as_white : context.is_white;
        print_board(&context, display_as_white);

        if (SHOW_EVALUATION) {
            printf("Evaluation (White): ");
            float raw_eval = evaluate(&context) * (context.is_white ? 1 : -1);
            if (raw_eval >= 0) {
                printf("+");
            }
            printf("%0.2f\n", raw_eval / 1000);
        }

        // printf("HASH: %lu %lu\n", context.hash.alpha, context.hash.beta);
        // printf("Repetitions: %u\n", state_repetitions(&history, context.hash));

        free(legal_moves.moves);
        legal_moves = get_all_legal_moves(&context);
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

        if (is_repetition_draw(&history.repetitions, context.hash)) {
            printf("Draw by repetition.\n");
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

        // Reset the board
        if (strcmp(input, "reset") == 0) {
            print_history(&history);
            clear_history(&history);
            new_context(&context);
            continue;
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

            if (depth > history.length) {
                printf("Depth (%u ply) exceeds history depth (%u ply).\n\n", depth, history.length);
                continue;
            }

            printf("%u move%s sucessfully undone:", depth, depth == 1 ? "" : "s");
            char move_str[6];
            for (int i = 0; i < depth; i++) {
                pop_history(&history, &context, move_str);
                printf(" %s", move_str);
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
        if (strcmp(input, "history") == 0) {
            print_history(&history);
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
                    new_context_branch(&context, &branch, legal_moves.moves[i]);
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
            BestMove best_move = get_best_move_ab(&history.repetitions, &context, MOVE_SEARCH_DEPTH);

            Piece piece = context.our_pieces[best_move.move.piece_id];

            char move_str[6];
            move_str[0] = piece.x + 'a';
            move_str[1] = piece.y + '1';
            move_str[2] = best_move.move.to_x + 'a';
            move_str[3] = best_move.move.to_y + '1';
            switch (best_move.move.special_move) {
                case PromoteKnight:
                    move_str[4] = 'n';
                    move_str[5] = '\0';
                    break;
                case PromoteBishop:
                    move_str[4] = 'b';
                    move_str[5] = '\0';
                    break;
                case PromoteRook:
                    move_str[4] = 'r';
                    move_str[5] = '\0';
                    break;
                case PromoteQueen:
                    move_str[4] = 'q';
                    move_str[5] = '\0';
                    break;
                default:
                    move_str[4] = '\0';
                    break;
            }

            append_history(&history, &context, move_str);
            printf(" %s\n\n", move_str);
            update_context(&context, best_move.move);
            continue;
        }

        // User should play this move
        bool found_move = false;
        for (int i = 0; i < legal_moves.n_moves; i++) {
            if (strcmp(legal_move_codes[i], input) == 0) {
                found_move = true;
                append_history(&history, &context, input);
                update_context(&context, legal_moves.moves[i]);
                printf("\n");
                break;
            }
        }
        if (!found_move) {
            printf("Please enter a valid move or command.\n\n");
        }
    }
    print_history(&history);
    free_history(&history);
    free(legal_moves.moves);
    free(legal_move_codes);
    return 0;
}