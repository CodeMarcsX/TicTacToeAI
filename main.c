#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

//CONSTANTS
// Colors
#define BOLD    "\033[1m"
#define RESET   "\033[0m"

#define CYAN    "\033[36m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

// Reinforcement Learning
#define TOTAL_STATES 19683 // Total number of possible game states
#define ALPHA 0.1 // Learning rate

// Training amount per difficulty level
#define EASY 500
#define NORMAL 3000
#define HARD 200000

// Global Variables
char board[9] = {' ',' ',' ',' ',' ',' ',' ',' ',' '};
char player = 'X';
float Q[TOTAL_STATES][9]; 

// Stores the game result and who won
typedef struct {
    char result;
    char playerWin;
} MatchResult;

// Starts the player vs player mode
static void gamePlayer();
// Makes the AI execute a move on the board
static void aiPlay();
// Starts an AI match
static void aiGame();
// Searches for a move that leads to victory for player 'p'
static int findWinningMove(char p, const char *current_board);
// Chooses the best move for the AI based on the current state
static int chooseMove(char me);
// Converts the current board state into a number (used in the Q-table)
static int getState(const char *current_board);
// Trains the AI by updating the Q-table based on state, move, and reward
static void trainAI(int state, int move, float reward);
// Automatically trains the AI several times (simulations) to improve learning
static void autoTrain(int level);
// Displays the board on the screen
static void printBoard(const char *current_board);
// Reads and validates the position entered by the player
static void readPosition();
// Checks if someone won or if there was a draw, returning a MatchResult struct
static MatchResult checkWinner(const char *current_board);
// Displays the main game menu
static void showMenu();
// Asks if the player wants to play again
static int playAgain();
// Resets the board (clears all positions)
static void resetBoard(const char *current_board);
// Shows text with a typewriter effect
static void typeWriter(const char *text, int delay_ms);
// Clears the terminal screen
static void clearScreen();

// Main
int main(void) {
    // This causes the text to be printed immediately on the terminal
    setbuf(stdout, NULL);
    srand(time(NULL));  

    showMenu();

    return 0;
}

static void gamePlayer() {
    resetBoard(board);

    printBoard(board);
    printf("\n");
    // shows Player turn
    printf(YELLOW BOLD"Player %c turn\n" RESET, player);

    while (1) {
        readPosition();
        // Switches turns after each move
        player = (player == 'X') ? 'O' : 'X';

        printf("\n");
        printf(YELLOW BOLD"Player %c turn\n" RESET, player);

        MatchResult conclusion = checkWinner(board);

        // Shows the result
        if (conclusion.result == 1) {
            printBoard(board);
            printf(GREEN BOLD "Player %c Won!\n" RESET, conclusion.playerWin);
            break;
        }
            
        else if (conclusion.result == 2) {
            printBoard(board);
            printf(RED BOLD "Draw!\n" RESET);
            break;
        }
    }
}

static void aiGame() {
    resetBoard(board);

    // Player starts
    player = 'X';

    MatchResult conclusion;

    while (1) {
        printBoard(board);

        // Player plays
        readPosition();

        // If there is already a winner, stop the loop
        conclusion = checkWinner(board);
        if (conclusion.result != 0)
            break;

        // AI plays
        aiPlay();
        printBoard(board);

        conclusion = checkWinner(board);
        if (conclusion.result != 0)
            break;
    }

    char *winner;
    // If the winner is 'O' then the AI won, otherwise the Player won
    winner = (conclusion.playerWin == 'O') ? "AI" : "Player";

    printBoard(board);

    // Shows results
    if (conclusion.result == 1) {
        printf(GREEN BOLD "%s Won!\n" RESET, winner);
    }
    else {
        printf(RED BOLD "Draw!\n" RESET);
    }
}

static void aiPlay() {
    int aiState;
    int aiMove;

    // Gives a 1-second delay for better interactivity
    sleep(1);

    // Transforms current board into a number
    aiState = getState(board);
    // Decides where to play
    aiMove = chooseMove('O');
    // Places the piece on the board
    board[aiMove] = 'O';
}


static int findWinningMove(char p, const char *current_board) {
    int i;
    char temp_board[9];

    for (i = 0; i < 9; i++)
        temp_board[i] = current_board[i];

    // Iterates through the entire board
    for (i = 0; i < 9; i++) {
        // Checks if it is empty
        if (temp_board[i] == ' ') {
            // Simulates move
            temp_board[i] = p;
            // Checks for victory
            MatchResult r = checkWinner(temp_board);
            // Undoes move
            temp_board[i] = ' ';
            // If it is a win, returns index
            if (r.result == 1 && r.playerWin == p)
                return i;
        }
    }

    // If there is no winning move
    return -1;
}

static int chooseMove(char me) {
    int state = getState(board);
    float best = -1e9;
    int bestMove = -1;
    int i;
    int move;

    // Checks who the opponent is
    char opp = (me == 'X') ? 'O' : 'X';

    // Tries to win
    int win = findWinningMove(me, board);
    if (win != -1)
        return win;

    // Blocks player
    int block = findWinningMove(opp, board);
    if (block != -1)
        return block;

    // A strategic shortcut for the AI, playing in the middle
    if(state == 0 && board[4] == ' ')
        return 4;

    // Iterates through the entire board
    for (i = 0; i < 9; i++) {
        // Checks if it is empty
        if (board[i] == ' ') {
            // To avoid choosing the same move when values are equal
            float noise = ((float)rand() / (float)RAND_MAX) * 0.01f; // Generates a random number, converts to float and reduces

            // Checks if this move is better than what was seen before
            if (Q[state][i] > best) {
                // If it is the best move, updates the best value and adds a little randomness (noise)
                best = Q[state][i] + noise; // Sums the best move value and the noise
                // Saves move
                bestMove = i;
            }
        }
    }

    // If the AI finds no good move or with a 10% chance, it chooses randomly
    if (bestMove == -1 || (rand() % 10 == 0)) {
        do {
            // Chooses randomly
            bestMove = rand() % 9;
        } while (board[bestMove] != ' '); // Ensures it is free
    }

    return bestMove;
}

static int getState(const char *current_board) {
    int state = 0;
    int i;

    // Iterates through the entire board
    for (i = 0; i < 9; i++) {
        int value = 0;
        // Transforms the position into a number according to the character occupying it
        if (current_board[i] == 'X')
            value = 1;
        else if (current_board[i] == 'O')
            value = 2;
        // Transforms the board into a unique number, making it easier for the AI to store in memory and use
        state = state * 3 + value; // Takes current number and adds a new board square
    }

    return state;
}

static void trainAI(int state, int move, float reward) {
    // Adjusts the move value based on the result
    Q[state][move] = Q[state][move] + ALPHA * (reward - Q[state][move]); // new value = old value + learning * error
}

static void autoTrain(int level) {
    // Clears previous training if it was done
    for (int i = 0; i < TOTAL_STATES; i++) {
        for (int j = 0; j < 9; j++) {
            Q[i][j] = 0.0f;
        }
    }

    clearScreen();
    typeWriter(GREEN BOLD "Training AI...\n" RESET, 50);

    // Performs the amount of training according to difficulty
    for(int i = 0; i < level; i++) {
        resetBoard(board);

        int states[9];
        int moves[9];
        int count = 0;
        char current = 'X';

        while(1) {

            int state = getState(board);
            int move;
            // Checks if it is the opponent's turn
            if (current == 'X') {
                // Gives a 50% chance for the move choice to be random or by the AI
                if (rand() % 2 == 0) {
                    do {
                        move = rand() % 9; // Random move
                    } while (board[move] != ' '); // Checks if it is empty
                }
                else
                    move = chooseMove(current); // AI move
            }
            else
                move = chooseMove(current); // If it is not the opponent's turn, it plays using the AI

            // Places 'X' or 'O' on the board
            board[move] = current;

            // If it is the AI's turn
            if (current == 'O') {
                // Saves the board state
                states[count] = state;
                // Saves which move the AI made
                moves[count] = move;
                // Allows storing moves, advancing to the next array position
                count++;
            }

            // Checks the winner
            MatchResult result = checkWinner(board);

            // IMPORTANT - System for AI learning, with reward and punishment
            if (result.result != 0) {
                float reward =  0;
                // If someone won
                if (result.result == 1) {
                    // If the AI won + 1 point, otherwise - 1 point
                    if (result.playerWin == 'O')
                        reward = 1.0f;
                    else
                        reward = -1.0f;
                }
                // In case of a draw + 0.3 points
                else {
                    reward = 0.3f;
                }

                int j;
                // Iterates through all moves
                for (j = 0; j < count; j ++) {
                    // Adds or subtracts points - Reinforcement learning
                    trainAI(states[j], moves[j], reward);
                    // Reduces previous rewards, as recent rewards are worth more
                    reward *= 0.9f; // Decreases 10% of previous rewards
                }
                
                break;
            }
            // Switches turns
            current = (current == 'X') ? 'O' : 'X';
        }
    }

    clearScreen();
    typeWriter(GREEN BOLD "Training done!\n" RESET, 50);
    sleep(1);
}

static void printBoard(const char *current_board) {
    clearScreen();

    printf(CYAN BOLD "  %c | %c | %c\n", current_board[0], current_board[1], current_board[2]);
    printf("----+---+----\n");
    printf("  %c | %c | %c\n", current_board[3], current_board[4], board[5]);
    printf("----+---+----\n");
    printf("  %c | %c | %c\n" RESET, current_board[6], current_board[7], current_board[8]);
}

// Used to convert the Player's move number into a place on the board
static void readPosition() {
    int pos = 0;

    // Verification so that the Player does not type anything beyond numbers and positions
    while (1) {
        printf("Choose a position (1-9): ");

        if (scanf("%d", &pos) != 1) { // Checks if character is a number
            printf(RED BOLD "Enter only numbers!\n" RESET);
            while(getchar() != '\n'); // Clears buffer
            continue;
        }

        pos--;

        // Checks if the place is out of bounds (1 to 9)
        if (pos < 0 || pos > 8) {
            printf(RED BOLD "Position out of bounds!\n" RESET);
            continue;
        }
        // Checks if the place is already filled
        if (board[pos] !=' ') {
            printf(RED BOLD "Position already filled\n" RESET);
            continue;
        }

        break;
    }

    // Places on the board
    board[pos] = player;
    printBoard(board);
}

static MatchResult checkWinner(const char *current_board) {
    MatchResult myBoard;
    myBoard.playerWin = ' ';
    myBoard.result = 0;
    int filled = 0;
    int i;
    int j;

    // Area that checks each part of the board looking for a winner
    for (i = 0; i < 3; i++) { // Checks all winning combinations
        // INDEX CALCULATION LOGIC
        
        // Loop index i (from 0 to 2) to calculate jumps of 3 (columns) and multiples of 3 (rows), mapping the 3x3 grid onto a linear array of 9 positions.
        // Vertical
        // The 3 board columns
        int a = i; // 1
        int b = a + 3; // 2
        int c = a + 6; // 3

        // Horizontal
        // The 3 board rows
        int d = i * 3; // 1
        int e = d + 1; // 2
        int f = d + 2; // 3

        // Vertical
        // Checks if column 1 equals 2 and column 2 equals 3 and column 1 is not empty
        if (current_board[a] == current_board[b] && current_board[b] == current_board[c] && current_board[a] != ' ') { // Looks for 3 identical characters aligned vertically
            // Changes result to 1, 1 = someone won
            myBoard.result = 1;
            // If aligned, gets the first character to see who won
            myBoard.playerWin = current_board[a];

            return myBoard;
        }

        // Horizontal
        // Checks if row 1 equals 2 and row 2 equals 3 and row 1 is not empty
        else if (current_board[d] == current_board[e] && current_board[e] == current_board[f] && current_board[d] != ' ') { // Looks for 3 identical characters aligned horizontally
            // Changes result to 1, 1 = someone won
            myBoard.result = 1;
            // If aligned, gets the first character to see who won
            myBoard.playerWin = current_board[d];

            return myBoard;
        }
    }

    // Left Diagonal
    // Checks if row 1 col 1 equals row 2 col 2 and row 2 col 2 equals col 3 and row 3 is not empty
    if (current_board[0] == current_board[4] && current_board[4] == current_board[8] && current_board[0] != ' ') { // Looks for 3 identical characters aligned diagonally
        // Changes result to 1, 1 = someone won
        myBoard.result = 1;
        // If aligned, gets the first character to see who won
        myBoard.playerWin = current_board[0];
    }
    
    // Right Diagonal
    // Checks if row 3 col 1 equals row 2 col 2 and row 2 col 2 equals row 3 col 1 not empty
    else if (current_board[2] == current_board[4] && current_board[4] == current_board[6] && current_board[2] != ' ') { // Looks for 3 identical characters aligned diagonally
        // Changes result to 1, 1 = someone won
        myBoard.result = 1;
        // If aligned, gets the first character to see who won
        myBoard.playerWin = current_board[2];
    }

    // Draw
    if (myBoard.result == 0) {  // Checks if all positions were filled without a winner
        for (j = 0; j < 9; j++) { // Iterates through the entire board
             if (current_board[j] != ' ') // Checks if the square is empty
                filled++; // Counts how many squares were filled
        }

        // If all squares were filled
        if (filled == 9)
            myBoard.result = 2; // Changes result to 2, 2 = draw
    }

    return myBoard;
}

static void showMenu() {
    while (1) {
        clearScreen();

        printf(
            BOLD GREEN
            "╔════════════════════════════╗\n"
            "║        TIC TAC TOE         ║\n"
            "╠════════════════════════════╣\n"
            "║  1  ▶  Player vs Player    ║\n"
            "║  2  ▶  Player vs AI        ║\n"
            "║" YELLOW "  3  ▶  Exit                " GREEN "║\n"
            "╚════════════════════════════╝\n"
            RESET
        );
        int option = 0;
        // Checks if the chosen option is between 1 and 3
        while (option < 1 || option > 3) {
            printf(GREEN BOLD"\nChoose option: " RESET);
    
            // Requests input and then checks if it is a number
            if (scanf("%d", &option) != 1) {
                printf(RED BOLD "Enter only numbers\n" RESET);
                while(getchar() != '\n'); // Clears buffer
    
                continue;
            }
        }
    
        // Option 1
        if(option == 1) {
            do {
                gamePlayer();
            } while (playAgain());
        }
    
        // Option 2
        else if(option == 2) {
            clearScreen();
    
            printf(
                BOLD GREEN
                "╔════════════════════════════╗\n"
                "║        Player vs AI        ║\n"
                "╠════════════════════════════╣\n"
                "║  1  ▶  Easy                ║\n"
                "║  2  ▶  Normal              ║\n"
                "║  3  ▶  Hard                ║\n"
                "║" YELLOW "  4  ▶  Back to Main Menu   " GREEN "║\n"
                "╚════════════════════════════╝\n"
                RESET
            );
            int optionAI = 0;
            // Checks if the chosen option is between 1 and 4
            while (optionAI < 1 || optionAI > 4) {
                printf(GREEN BOLD"\nChoose option: " RESET);
    
                // Requests input and then checks if it is a number
                if (scanf("%d", &optionAI) != 1) {
                    printf(RED BOLD "Enter only numbers\n" RESET);
                    while(getchar() != '\n'); // Clears buffer
    
                    continue;
                }
            }
    
            // AI Option 1
            if (optionAI == 1) {
                autoTrain(EASY);
                do {
                    aiGame();
                } while (playAgain());
            }
    
            // AI Option 2
            else if (optionAI == 2) {
                autoTrain(NORMAL);
                do {
                    aiGame();
                } while (playAgain());
            }
    
            // AI Option 3
            else if (optionAI == 3) {
                autoTrain(HARD);
                do {
                    aiGame();
                } while (playAgain());
            }
    
            // AI Option 4
            else if (optionAI == 4)
                showMenu();
        }
    
        // Option 3
        else if(option == 3) {
            clearScreen();
            // Farewell
            typeWriter(BOLD "Thanks for playing!\n" RESET, 50);

            break;
        }
    }
}

static int playAgain() {
    char again;

    // Asks and requests input
    printf("Play again? (y/n): ");
    scanf(" %c", &again);

    // Clear buffer
    while(getchar() != '\n');

    // Converts letter to lowercase
    return (tolower(again) == 'y');
}

static void resetBoard(const char *current_board) {
    int i;

    // Iterates through the entire board
    for (i = 0; i < 9; i ++)
        board[i] = ' '; // Changes character to ' '

    player = 'X'; // Player starts
}

static void typeWriter(const char *text, int delay_ms) {
    // Types each letter with a defined delay
    for (int i = 0; text[i] != '\0'; i++) {
        printf("%c", text[i]);
        fflush(stdout); // Print to screen immediately
        usleep(delay_ms * 1000); // The delay
    }
}

static void clearScreen() {
    // Platform-specific screen clearing
    #if defined(_WIN32)
        system("cls");
    #elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
        system("clear");
    #endif
}
