#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#define BOLD    "\033[1m"
#define RESET   "\033[0m"

#define CYAN    "\033[36m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

char board[9] = {' ',' ',' ',' ',' ',' ',' ',' ',' '};
char player = 'X';
float Q[19683][9];
int aiState;
int aiMove;
float alpha = 0.1;

#define EASY 500
#define NORMAL 3000
#define HARD 200000

typedef struct {
    char result;
    char playerWin;
} Table;

void gamePlayer();
void aiPlay();
void aiGame();
int findWinningMove(char p);
int chooseMove(char me);
int getState();
void trainAI(int state, int move, float reward);
void autoTrain(int level);
void printBoard();
void readPosition();
Table checkWinner();
void showMenu();
void playAgain(int game);
void resetBoard();
void typeWriter(const char *text, int delay_ms);
void clearScreen();

int main(void) {
    // This causes the text to be printed immediately on the terminal
    setbuf(stdout, NULL);
    srand(time(NULL));  

    showMenu();

    return 0;
}

void gamePlayer() {
    resetBoard();

    printBoard();
    printf("\n");
    printf(YELLOW BOLD"Player %c turn\n" RESET, player);

    while (1) {
        readPosition();
        player = (player == 'X') ? 'O' : 'X';

        printf("\n");
        printf(YELLOW BOLD"Player %c turn\n" RESET, player);

        Table conclusion = checkWinner();

        if (conclusion.result == 1) {
            printBoard();
            printf(GREEN BOLD "Player %c Won!\n" RESET, conclusion.playerWin);

            playAgain(2);

            break;
        }
            
        else if (conclusion.result == 2) {
            printBoard();
            printf(RED BOLD "Draw!\n" RESET);

            playAgain(2);

            break;
        }
    }
}

void aiGame() {
    resetBoard();

    player = 'X';

    Table conclusion;

    while (1) {
        printBoard();

        readPosition();

        conclusion = checkWinner();
        if (conclusion.result != 0)
            break;

        aiPlay();
        printBoard();

        conclusion = checkWinner();
        if (conclusion.result != 0)
            break;
    }

    char *winner;
    winner = (conclusion.playerWin == 'O') ? "AI" : "Player";

    printBoard();

    if (conclusion.result == 1) {
        printf(GREEN BOLD "%s Won!\n" RESET, winner);
        playAgain(1);
    }
    else {
        printf(RED BOLD "Draw!\n" RESET);
        playAgain(1);
    }
}

void aiPlay() {
    sleep(1);

    aiState = getState();
    aiMove = chooseMove('O');

    board[aiMove] = 'O';
}

int findWinningMove(char p) {
    int i;

    for (i = 0; i < 9; i++) {
        if (board[i] == ' ') {
            board[i] = p;

            Table r = checkWinner();

            board[i] = ' ';

            if (r.result == 1 && r.playerWin == p)
                return i;
        }
    }

    return -1;
}

// It makes the AI ​​choose the best position.
int chooseMove(char me) {
    int state = getState();
    float best = -1e9;
    int bestMove = -1;
    int i;
    int move;

    char opp = (me == 'X') ? 'O' : 'X';

    int win = findWinningMove(me);
    if (win != -1)
        return win;

    int block = findWinningMove(opp);
    if (block != -1)
        return block;

    if(state == 0 && board[4] == ' ')
        return 4;

    for (i = 0; i < 9; i++) {
        if (board[i] == ' ') {
            float noise = ((float)rand() / (float)RAND_MAX) * 0.01f;

            if (Q[state][i] > best) {
                best = Q[state][i] + noise;
                bestMove = i;
            }
        }
    }

    if (bestMove == -1 || (rand() % 10 == 0)) {
        do {
            bestMove = rand() % 9;
        } while (board[bestMove] != ' ');
    }

    return bestMove;
}

int getState() {
    int state = 0;
    int i;
    for (i = 0; i < 9; i++) {
        int value = 0;

        if (board[i] == 'X')
            value = 1;
        else if (board[i] == 'O')
            value = 2;
        state = state * 3 + value;
    }

    return state;
}

void trainAI(int state, int move, float reward) {
    Q[state][move] = Q[state][move] + alpha * (reward - Q[state][move]);
}

void autoTrain(int level) {
    for (int i = 0; i < 19683; i++) {
        for (int j = 0; j < 9; j++) {
            Q[i][j] = 0.0f;
        }
    }

    clearScreen();
    typeWriter(GREEN BOLD "Training AI...\n" RESET, 50);

    for(int i = 0; i < level; i++) {
        resetBoard();

        int states[9];
        int moves[9];
        int count = 0;
        char current = 'X';

        while(1) {

            int state = getState();
            int move;

            if (current == 'X') {
                if (rand() % 2 == 0) {
                    do {
                        move = rand() % 9;
                    } while (board[move] != ' ');
                }
                else
                    move = chooseMove(current);
            }
            else
                move = chooseMove(current);

            board[move] = current;

            if (current == 'O') {
                states[count] = state;
                moves[count] = move;
                count++;
            }

            Table result = checkWinner();

            if (result.result != 0) {
                float reward =  0;

                if (result.result == 1) {
                    if (result.playerWin == 'O')
                        reward = 1.0f;
                    else
                        reward = -1.0f;
                }

                else {
                    reward = 0.3f;
                }

                int j;
                for (j = 0; j < count; j ++) {
                    trainAI(states[j], moves[j], reward);
                    reward *= 0.9f;
                }
                
                break;
            }

            current = (current == 'X') ? 'O' : 'X';
        }
    }

    clearScreen();
    typeWriter(GREEN BOLD "Training done!\n" RESET, 50);
    sleep(1);
}

void printBoard() {
    clearScreen();

    printf(CYAN BOLD "  %c | %c | %c\n", board[0], board[1], board[2]);
    printf("----+---+----\n");
    printf("  %c | %c | %c\n", board[3], board[4], board[5]);
    printf("----+---+----\n");
    printf("  %c | %c | %c\n" RESET, board[6], board[7], board[8]);
}

void readPosition() {
    int pos = 0;

    while (pos < 1 || pos > 9) {
        printf("Choose a position (1-9): ");

        if (scanf("%d", &pos) != 1) {
            printf(RED BOLD "Enter only numbers!\n" RESET);
            while(getchar() != '\n');
            continue;
        }
    }

    pos--;

    while (board[pos] != ' ') {
        printBoard();
        printf(RED BOLD "Position already filled\n" RESET);

        pos = 0;

        while (pos < 1 || pos > 9) {
            printf("Choose a position (1-9): ");

            if (scanf("%d", &pos) != 1)
                printf(RED BOLD "Invalid input\n" RESET);

            pos--;
        }
    }

    board[pos] = player;

    printBoard();
}

Table checkWinner() {
    Table myBoard;
    myBoard.playerWin = ' ';
    myBoard.result = 0;
    int filled = 0;
    int i;
    int j;

    for (i = 0; i < 3; i++) {
        // Vertical
        int a = i;
        int b = a + 3;
        int c = a + 6;

        // Horizontal
        int d = i * 3;
        int e = d + 1;
        int f = d + 2;

        // Vertical
        if (board[a] == board[b] && board[b] == board[c] && board[a] != ' ') {
            myBoard.result = 1;
            myBoard.playerWin = board[a];

            return myBoard;
        }

        // Horizontal
        else if (board[d] == board[e] && board[e] == board[f] && board[d] != ' ') {
            myBoard.result = 1;
            myBoard.playerWin = board[d];

            return myBoard;
        }
    }

    // Diagonal Esquerda
    if (board[0] == board[4] && board[4] == board[8] && board[0] != ' ') {
        myBoard.result = 1;
        myBoard.playerWin = board[0];
    }
    
    // Diagonal Direito
    else if (board[2] == board[4] && board[4] == board[6] && board[2] != ' ') {
        myBoard.result = 1;
        myBoard.playerWin = board[2];
    }

    // Empate
    if (myBoard.result == 0) {
        for (j = 0; j < 9; j++) {
             if (board[j] != ' ')
                filled++;
        }

        if (filled == 9)
            myBoard.result = 2;
    }

    return myBoard;
}

void showMenu() {
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
    while (option < 1 || option > 3) {
        printf(GREEN BOLD"\nChoose option: " RESET);

        if (scanf("%d", &option) != 1) {
            printf(RED BOLD "Enter only numbers\n" RESET);
            while(getchar() != '\n');

            continue;
        }
    }

    if(option == 1)
        gamePlayer();

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
        while (optionAI < 1 || optionAI > 4) {
            printf(GREEN BOLD"\nChoose option: " RESET);

            if (scanf("%d", &optionAI) != 1) {
                printf(RED BOLD "Enter only numbers\n" RESET);
                while(getchar() != '\n');

                continue;
            }
        }

        if (optionAI == 1) {
            autoTrain(EASY);
            aiGame();
        }
        else if (optionAI == 2) {
            autoTrain(NORMAL);
            aiGame();
        }
        else if (optionAI == 3) {
            autoTrain(HARD);
            aiGame();
        }
        else if (optionAI == 4)
            showMenu();
    }

    else if(option == 3) {
        clearScreen();
        typeWriter(BOLD "Thanks for playing!\n" RESET, 50);
    }
}

void playAgain(int game) {
    char again;

    printf("Play again? (y/n): ");
    scanf(" %c", &again);

    again = tolower(again);

    if (again == 'n') {
        clearScreen();
        showMenu();
    }
    else if (again == 'y') {
        if (game == 1)
            aiGame();
        else if (game == 2)
            gamePlayer();
    }
    else {
        setbuf(stdin, NULL);
        printf("Option Invalid\n");
        playAgain(game);
    }
}

void resetBoard() {
    int i;

    for (i = 0; i < 9; i ++)
        board[i] = ' ';

    player = 'X';
}

void typeWriter(const char *text, int delay_ms) {
    for (int i = 0; text[i] != '\0'; i++) {
        printf("%c", text[i]);
        fflush(stdout);
        usleep(delay_ms * 1000);
    }
}

void clearScreen() {
    // Platform-specific screen clearing
    #if defined(_WIN32)
        system("cls");
    #elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
        system("clear");
    #endif
}
