#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h> 

#include "error_logger.h"

#define NUM_STATES 19683
#define NUM_ACTIONS 9
#define NUM_CELLS 9

static const int powers_of_3[9] = {1, 3, 9, 27, 81, 243, 729, 2187, 6561};

/*
empty = 0
    X = 1
    O = 2
*/
typedef struct {
    int cells[NUM_CELLS];
} Board;

typedef struct {
    int state;
    int action;
} Move;

typedef struct {
    float Q[NUM_STATES][NUM_ACTIONS];
    float epsilon;
    float alpha;
} Agent;

Board new_board() {
    Board b = {.cells = {0}};
    return b;
}

int encode_board(Board* b) {
    int id = 0;
    for (int i = 0; i < NUM_CELLS; i++) {
        id += b->cells[i] * powers_of_3[i];
    }
    return id;
}

Agent* create_agent(float epsilon, float alpha) {
    Agent* a = calloc(1, sizeof(*a));
    if (!a) {
        error("ERROR: create_agent() failed to calloc(1, sizeof(*a)).");
        exit(1);
    }

    a->epsilon = epsilon;
    a->alpha = alpha;
    return a;
}

void free_agent(Agent* a) {
    if (!a) return;
    free(a);
}

int load_Q_table(Agent* a, const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return 0; // File does not exist

    fread(a->Q, sizeof(float), NUM_STATES * NUM_ACTIONS, f);
    fclose(f);
    return 1; // Loaded the file successfully
}

int save_Q_table(Agent* a, const char* filename) {
    FILE* f = fopen(filename, "wb");
    if (!f) return 0; // Failed to create file

    fwrite(a->Q, sizeof(float), NUM_STATES * NUM_ACTIONS, f);

    fclose(f);
    return 1; // Successfully saved the file
}

int get_available_moves(Board* b, int moves[9]) {
    int count = 0;
    for (int i = 0; i < NUM_CELLS; i++) {
        if (b->cells[i] == 0) {
            moves[count++] = i;
        }
    }
    return count;
}

// epsilon-greedy choose move
int choose_move(Agent* a, Board* b) {
    int available_moves[9] = {0};
    int available_moves_count = get_available_moves(b, available_moves);

    if (available_moves_count == 0) {
        error("ERROR: choose_move() there are no available_move; available_moves_count is zero.");
        exit(1);
    }

    int state = encode_board(b);

    if ((float)rand()/RAND_MAX < a->epsilon) {
        return available_moves[rand() % available_moves_count];
    }

    float best = -1e9;
    int best_move = available_moves[0];

    for (int i = 0; i < available_moves_count; i++) {
        int move = available_moves[i];

        if (a->Q[state][move] > best) {
            best = a->Q[state][move];
            best_move = move;
        }
    }
    
    return best_move;
}

/*
    "int p" is simply X or O for the players
    X = 1
    O = 2
    Same as the Board struct except players cant put empty = 0
*/
void apply_move(Board* b, int move, int p) {
    b->cells[move] = p;
}

void update_Q_values(Agent* a, Move* h, int h_count, float reward) {
    for (int i = h_count - 1; i >= 0; i--) {
        int state = h[i].state;
        int action = h[i].action;

        a->Q[state][action] += a->alpha * (reward - a->Q[state][action]);
    }
}

int check_winner(Board* b) {
    int wins[8][3] = {
        {0,1,2}, {3,4,5}, {6,7,8}, // rows
        {0,3,6}, {1,4,7}, {2,5,8}, // cols
        {0,4,8}, {2,4,6}           // diagonals
    };

    for (int i = 0; i < 8; i++) {
        int a = wins[i][0];
        int d = wins[i][1];
        int e = wins[i][2];

        if (b->cells[a] != 0 &&
            b->cells[a] == b->cells[d] &&
            b->cells[a] == b->cells[e]) {

            return b->cells[a]; // 1 or 2
        }
    }

    // check tie
    for (int i = 0; i < 9; i++) {
        if (b->cells[i] == 0)
            return 0; // game not finished
    }

    return 3; // tie
}

void train() {
    float epsilon = 1.0f;
    float epsilon_min   = 0.01f;
    float epsilon_decay = 0.9999999f;
    float alpha = 0.1f;

    Agent* a1 = create_agent(epsilon, alpha);

    if (!load_Q_table(a1, "agent1.bin")) {
        printf("Starting new agent a1\n");
    }

    int wins1 = 0;
    int wins2 = 0;
    int ties = 0;
    for (int i = 0; i < 100000000; i++) {
        if (i % 1000000 == 0) {
            printf("Game %d | epsilon=%f | X:%d O:%d T:%d\n",
                i, a1->epsilon, wins1, wins2, ties);

            wins1 = 0;
            wins2 = 0;
            ties = 0;
        }
        // Players can only make maximum 5 to 4 moves
        Move history1[5];
        Move history2[5];
        int h1_count = 0;
        int h2_count = 0;

        bool a1_turn = rand() % 2;
        Board board = new_board();

        bool game_over = false;
        while (!game_over) {
            int state = encode_board(&board);

            if (a1_turn) {
                int action = choose_move(a1, &board);
                history1[h1_count++] = (Move){state, action};
                apply_move(&board, action, 1);  // X
            } else {
                int action = choose_move(a1, &board);
                history2[h2_count++] = (Move){state, action};
                apply_move(&board, action, 2);  // O
            }

            int result = check_winner(&board);
            if (result != 0) {
                game_over = true;

                float reward1 = 0.5;
                float reward2 = 0.5;

                if (result == 1) {
                    reward1 = 1;
                    reward2 = -1;
                    wins1++;
                } 
                else if (result == 2) {
                    reward1 = -1;
                    reward2 = 1;
                    wins2++;
                }
                else {
                    ties++;
                }

                update_Q_values(a1, history1, h1_count, reward1);
                update_Q_values(a1, history2, h2_count, reward2);
            }

            a1_turn = !a1_turn;
        }

        a1->epsilon *= epsilon_decay;
        if (a1->epsilon < epsilon_min) a1->epsilon = epsilon_min;
    }

    save_Q_table(a1, "agent1.bin");
    free_agent(a1);
}

void print_board(Board* b) {
    for (int i = 0; i < 9; i++) {
        char c = '.';

        if (b->cells[i] == 1) c = 'X';
        if (b->cells[i] == 2) c = 'O';

        printf(" %c ", c);

        if (i % 3 != 2) printf("|");
        if (i % 3 == 2 && i != 8)
            printf("\n---+---+---\n");
    }

    printf("\n\n");
}

int human_move(Board* b) {
    int move;

    while (1) {
        printf("Enter move (0-8): ");
        scanf("%d", &move);

        if (move < 0 || move > 8) {
            printf("Invalid square.\n");
            continue;
        }

        if (b->cells[move] != 0) {
            printf("Square already taken.\n");
            continue;
        }

        return move;
    }
}

void play() {
    Agent* agent = create_agent(0.3f, 0.0f); // no exploration

    if (!load_Q_table(agent, "agent1.bin")) {
        printf("No trained agent found.\n");
        return;
    }

    Board board = new_board();

    bool human_turn = true;

    printf("You are X\n");

    bool game_over = false;

    while (!game_over) {
        print_board(&board);

        if (human_turn) {
            int move = human_move(&board);
            apply_move(&board, move, 1);
        } 
        else {
            int move = choose_move(agent, &board);
            printf("Agent plays: %d\n", move);
            apply_move(&board, move, 2);
        }

        int result = check_winner(&board);

        if (result != 0) {
            print_board(&board);

            if (result == 1)
                printf("You win!\n");
            else if (result == 2)
                printf("Agent wins!\n");
            else
                printf("Tie!\n");

            game_over = true;
        }

        human_turn = !human_turn;
    }

    free_agent(agent);
}

/*
Agent* easy = create_agent(0.3f, 0.0f);  // 30% random
Agent* medium = create_agent(0.1f, 0.0f); // 10% random
Agent* hard = create_agent(0.0f, 0.0f);  // perfect
*/

int main(int argc, char* argv[]) {
    // Seed randomness
    srand(time(NULL));

    switch (argc) {
        case 2:
            if (strcmp(argv[1], "play") == 0) {
                play();
            }
            else if (strcmp(argv[1], "train") == 0) {
                train();
            }
            else {
                error("Invalid arguments");
                warning("Try \"./tictactoe play\" or \"./tictactoe train\"\n");
            }
            break;

        default:
            error("Invalid argument count");
            warning("Try \"./tictactoe play\" or \"./tictactoe train\"\n");
    }

    return 0;
}