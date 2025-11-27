#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
  #include <windows.h> // optionally for Sleep if needed
#else
  #include <unistd.h>
#endif

char board[3][3];
char currentMarker;
int currentPlayer;
int gameMode; // 1 = PvP, 2 = PvE

void resetBoard();
void printBoard();
int placeMarker(int row, int col);
int checkWin();
void switchPlayer();
void clearScreen();
void aiMove();  // Machine move logic

// AI helpers
int tryWinOrBlock(char marker);
int randomMove();

// Utility
void readLine(char *buf, int size);
void flushStdin();

int mainMenu();
void playGame();

int main() {
    srand((unsigned)time(NULL));
    while (1) {
        clearScreen();

        // ASCII art in green (ANSI) if terminal supports it
        printf("\033[0;32m"); // GREEN
        printf("88P'888'Y88 ,e,                  88P'888'Y88                          88P'888'Y88                     \n");
        printf("P'  888  'Y  \"   e88'888         P'  888  'Y  ,\"Y88b  e88'888         P'  888  'Y  e88 88e   ,e e,    \n");
        printf("    888     888 d888  '8   888       888     \"8\" 888 d888  '8   888       888     d888 888b d88 88b   \n");
        printf("    888     888 Y888   ,             888     ,ee 888 Y888   ,             888     Y888 888P 888   ,   \n");
        printf("    888     888  \"88,e8'             888     \"88 888  \"88,e8'             888      \"88 88\"   \"YeeP\"   \n");
        printf("                                                                                                      \n");
        printf("                                                                                                      \n");
        printf("\033[0m"); // RESET

        printf("\nPress ENTER to continue...");
        getchar();

        int choice = mainMenu();
        if (choice == 3) {
            clearScreen();
            printf("Goodbye!\n");
            break;
        }
        // set gameMode based on main menu
        gameMode = (choice == 1) ? 1 : 2;
        playGame();
    }

    return 0;
}

// Show main menu and return 1=PvP,2=PvE,3=Exit
int mainMenu() {
    int choice = 0;
    while (1) {
        clearScreen();
        printf("========== TIC-TAC-TOE MAIN MENU ==========\n\n");
        printf("1. Player vs Player\n");
        printf("2. Player vs Machine (AI)\n");
        printf("3. Exit\n\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Try again.\n");
            flushStdin();
            continue;
        }
        flushStdin();
        if (choice >= 1 && choice <= 3) return choice;
        printf("Please choose 1, 2 or 3. Press ENTER to retry...");
        getchar();
    }
}

// The main play loop; handles names, rounds, replay/main menu/exit
void playGame() {
    char player1Name[64] = "Player1";
    char player2Name[64] = "Player2";
    int continueSession = 1; // 1 = play round, 0 = return to main menu or exit
    int sessionAction = 1;   // 1 = play again, 2 = main menu, 3 = exit

    // Get player names depending on mode
    clearScreen();
    if (gameMode == 1) {
        printf("Player vs Player selected.\n");
        printf("Enter name for Player 1: ");
        readLine(player1Name, sizeof(player1Name));
        if (strlen(player1Name) == 0) strcpy(player1Name, "Player1");

        printf("Enter name for Player 2: ");
        readLine(player2Name, sizeof(player2Name));
        if (strlen(player2Name) == 0) strcpy(player2Name, "Player2");
    } else {
        printf("Player vs Machine selected.\n");
        printf("Enter your name: ");
        readLine(player1Name, sizeof(player1Name));
        if (strlen(player1Name) == 0) strcpy(player1Name, "Player");
        strcpy(player2Name, "Machine");
    }

    // loop rounds until user chooses otherwise
    while (1) {
        int row, col;
        int winner = 0;
        int moves = 0;

        resetBoard();
        currentPlayer = 1;
        currentMarker = 'X';

        while (1) {
            clearScreen();
            printf("============================\n");
            printf("         TIC-TAC-TOE        \n");
            printf("============================\n\n");

            printBoard();

            // Show whose turn: print name and player number
            if (gameMode == 2 && currentPlayer == 2) {
                // Machine's turn
                printf("\n%s (Player %d) (%c) is thinking...\n", player2Name, currentPlayer, currentMarker);
                aiMove();
            } else {
                // Human turn (either PvP both humans or PvE human)
                const char *nameToShow = (currentPlayer == 1) ? player1Name : player2Name;
                printf("\n%s (Player %d) (%c), enter row and column (1-3): ",
                       nameToShow, currentPlayer, currentMarker);

                if (scanf("%d %d", &row, &col) != 2) {
                    printf("Invalid input! Try again.\n");
                    flushStdin();
                    printf("Press ENTER to continue...");
                    getchar();
                    continue;
                }
                flushStdin();

                if (row < 1 || row > 3 || col < 1 || col > 3) {
                    printf("Invalid move! Try again.\n");
                    printf("Press ENTER to continue...");
                    getchar();
                    continue;
                }

                if (!placeMarker(row - 1, col - 1)) {
                    printf("Spot already taken! Try again.\n");
                    printf("Press ENTER to continue...");
                    getchar();
                    continue;
                }
            }

            moves++;
            winner = checkWin();

            if (winner == 1) {
                clearScreen();
                printBoard();
                const char *winnerName = (currentPlayer == 1) ? player1Name : player2Name;
                printf("\n====================================\n");
                printf(" Congratulations, %s! You won! \n", winnerName);
                printf(" Well played, Player %d (%c).\n", currentPlayer, currentMarker);
                printf("====================================\n");
                break;
            } else if (moves == 9) {
                clearScreen();
                printBoard();
                printf("\n====================================\n");
                printf(" It's a draw! Nice game.\n");
                printf("====================================\n");
                break;
            }

            switchPlayer();
        }

        // After round menu: Play again / Main menu / Exit
        sessionAction = 0;
        while (1) {
            printf("\nWhat next?\n");
            printf("1. Play Again (same mode & names)\n");
            printf("2. Main Menu\n");
            printf("3. Exit\n");
            printf("Enter choice: ");
            if (scanf("%d", &sessionAction) != 1) {
                printf("Invalid input. Try again.\n");
                flushStdin();
                continue;
            }
            flushStdin();
            if (sessionAction >= 1 && sessionAction <= 3) break;
            printf("Please choose 1-3.\n");
        }

        if (sessionAction == 1) {
            // continue loop => play again with same players & mode
            continue;
        } else if (sessionAction == 2) {
            // go back to main menu
            return;
        } else {
            // exit whole program
            clearScreen();
            printf("Thanks for playing. Goodbye!\n");
            exit(0);
        }
    }
}

// ======================= FUNCTIONS ============================

void resetBoard() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            board[i][j] = ' ';
}

void printBoard() {
    printf("     1   2   3\n");
    printf("   -------------\n");
    for (int i = 0; i < 3; i++) {
        printf(" %d |", i + 1);
        for (int j = 0; j < 3; j++) {
            printf(" %c |", board[i][j]);
        }
        printf("\n   -------------\n");
    }
}

int placeMarker(int row, int col) {
    if (board[row][col] != ' ')
        return 0;
    board[row][col] = currentMarker;
    return 1;
}

int checkWin() {
    for (int i = 0; i < 3; i++) {
        if ((board[i][0] == currentMarker &&
             board[i][1] == currentMarker &&
             board[i][2] == currentMarker) ||
            (board[0][i] == currentMarker &&
             board[1][i] == currentMarker &&
             board[2][i] == currentMarker))
            return 1;
    }

    if ((board[0][0] == currentMarker &&
         board[1][1] == currentMarker &&
         board[2][2] == currentMarker) ||
        (board[0][2] == currentMarker &&
         board[1][1] == currentMarker &&
         board[2][0] == currentMarker))
        return 1;

    return 0;
}

void switchPlayer() {
    currentPlayer = (currentPlayer == 1) ? 2 : 1;
    currentMarker = (currentMarker == 'X') ? 'O' : 'X';
}

void clearScreen() {
    fflush(stdout);
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ========================= MACHINE LOGIC ==========================

// Try to win or block. Returns 1 if move done.
int tryWinOrBlock(char marker) {
    for (int i = 0; i < 3; i++) {
        // Rows
        if (board[i][0] == marker &&
            board[i][1] == marker &&
            board[i][2] == ' ') { currentMarker = 'O'; return placeMarker(i, 2); }

        if (board[i][0] == marker &&
            board[i][2] == marker &&
            board[i][1] == ' ') { currentMarker = 'O'; return placeMarker(i, 1); }

        if (board[i][1] == marker &&
            board[i][2] == marker &&
            board[i][0] == ' ') { currentMarker = 'O'; return placeMarker(i, 0); }

        // Columns
        if (board[0][i] == marker &&
            board[1][i] == marker &&
            board[2][i] == ' ') { currentMarker = 'O'; return placeMarker(2, i); }

        if (board[0][i] == marker &&
            board[2][i] == marker &&
            board[1][i] == ' ') { currentMarker = 'O'; return placeMarker(1, i); }

        if (board[1][i] == marker &&
            board[2][i] == marker &&
            board[0][i] == ' ') { currentMarker = 'O'; return placeMarker(0, i); }
    }

    // Diagonals
    if (board[0][0] == marker &&
        board[1][1] == marker &&
        board[2][2] == ' ') { currentMarker = 'O'; return placeMarker(2, 2); }

    if (board[0][0] == marker &&
        board[2][2] == marker &&
        board[1][1] == ' ') { currentMarker = 'O'; return placeMarker(1, 1); }

    if (board[1][1] == marker &&
        board[2][2] == marker &&
        board[0][0] == ' ') { currentMarker = 'O'; return placeMarker(0, 0); }

    // Anti-diagonal
    if (board[0][2] == marker &&
        board[1][1] == marker &&
        board[2][0] == ' ') { currentMarker = 'O'; return placeMarker(2, 0); }

    if (board[0][2] == marker &&
        board[2][0] == marker &&
        board[1][1] == ' ') { currentMarker = 'O'; return placeMarker(1, 1); }

    if (board[1][1] == marker &&
        board[2][0] == marker &&
        board[0][2] == ' ') { currentMarker = 'O'; return placeMarker(0, 2); }

    return 0;
}

// Random empty spot
int randomMove() {
    int empty[9][2];
    int count = 0;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] == ' ') {
                empty[count][0] = i;
                empty[count][1] = j;
                count++;
            }

    if (count == 0) return 0;

    int choice = rand() % count;
    currentMarker = 'O';
    return placeMarker(empty[choice][0], empty[choice][1]);
}

void aiMove() {
    // AI uses 'O' marker when playing as player 2
    // Try winning first (for 'O')
    if (tryWinOrBlock('O')) return;

    // Try blocking player (block 'X')
    if (tryWinOrBlock('X')) return;

    // Otherwise random
    randomMove();
}

// ========================= UTILITIES ============================
void readLine(char *buf, int size) {
    if (fgets(buf, size, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    // strip newline
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
}

void flushStdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}
