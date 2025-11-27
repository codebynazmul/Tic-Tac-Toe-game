#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char board[3][3];
char currentMarker;
int currentPlayer;

void resetBoard();
void printBoard();
int placeMarker(int row, int col);
int checkWin();
void switchPlayer();
void clearScreen();

int main() {
    int row, col;
    int winner = 0;
    int moves = 0;

    resetBoard();
    currentPlayer = 1;
    currentMarker = 'X';

    clearScreen();

    // ============================
    //       ASCII ART OPENING
    // ============================
printf("88P'888'Y88 ,e,                  88P'888'Y88                          88P'888'Y88                     \n");
printf("P'  888  'Y  \"   e88'888         P'  888  'Y  ,\"Y88b  e88'888         P'  888  'Y  e88 88e   ,e e,    \n");
printf("    888     888 d888  '8   888       888     \"8\" 888 d888  '8   888       888     d888 888b d88 88b   \n");
printf("    888     888 Y888   ,             888     ,ee 888 Y888   ,             888     Y888 888P 888   ,   \n");
printf("    888     888  \"88,e8'             888     \"88 888  \"88,e8'             888      \"88 88\"   \"YeeP\"   \n");
printf("                                                                                                      \n");
printf("                                                                                                      \n");

    printf("\nPress ENTER to start the game...");
    getchar();

    while (1) {
        clearScreen();
        printf("============================\n");
        printf("      TIC-TAC-TOE GAME       \n");
        printf("============================\n\n");
        printBoard();

        printf("\nPlayer %d (%c), enter row and column (1-3): ", currentPlayer, currentMarker);
        if (scanf("%d %d", &row, &col) != 2) {
            printf("Invalid input! Try again.\n");
            getchar(); getchar();
            continue;
        }

        if (row < 1 || row > 3 || col < 1 || col > 3) {
            printf("Invalid move! Try again.\n");
            getchar(); getchar();
            continue;
        }

        if (!placeMarker(row - 1, col - 1)) {
            printf("Spot already taken! Try again.\n");
            getchar(); getchar();
            continue;
        }

        moves++;
        winner = checkWin();

        if (winner == 1) {
            clearScreen();
            printBoard();
            printf("\n Player %d (%c) wins!\n", currentPlayer, currentMarker);
            break;
        } else if (moves == 9) {
            clearScreen();
            printBoard();
            printf("\n It's a draw!\n");
            break;
        }

        switchPlayer();
    }

    printf("\n====================================\n");
    printf("  Developed by MD NAZMUL HASAN     \n");
    printf("====================================\n");
    return 0;
}

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
        if ((board[i][0] == currentMarker && board[i][1] == currentMarker && board[i][2] == currentMarker) ||
            (board[0][i] == currentMarker && board[1][i] == currentMarker && board[2][i] == currentMarker))
            return 1;
    }

    if ((board[0][0] == currentMarker && board[1][1] == currentMarker && board[2][2] == currentMarker) ||
        (board[0][2] == currentMarker && board[1][1] == currentMarker && board[2][0] == currentMarker))
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
