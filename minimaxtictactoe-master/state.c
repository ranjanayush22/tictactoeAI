#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "state.h"

Board initBoard() {
    /* Initialize an empty board. */

    Board b;
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            b.tiles[i][j] = EMPTY;
        }
    }

    return b;
}

State initState() {
    /* Initialize a new game state. */

    State s;
    s.board = initBoard();
    s.playerToMove = X;

    return s;
}

int checkWin(State s) {
    /*
    Return X if player X won or O if player O won.
    Return -1 if there was a draw or the game isn't over.
    */

    Board b = s.board;

    const int winX = pow(X, BOARD_SIZE);
    const int winO = pow(O, BOARD_SIZE);

    // FIXME won't change if board size changes
    int rowProduct[] = {1, 1, 1};
    int colProduct[] = {1, 1, 1};
    int diagProduct[] = {1, 1};

    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            rowProduct[i] *= b.tiles[i][j];
            colProduct[j] *= b.tiles[i][j];

            // TODO fix diagonal product
            if(i == j) {
                diagProduct[0] *= b.tiles[i][j];
            } 
            if ((i + j) == (BOARD_SIZE - 1)) {
                diagProduct[1] *= b.tiles[i][j];
            }
        }
    }

    for(int k = 0; k < BOARD_SIZE; k++) {
        if(rowProduct[k] == winX || colProduct[k] == winX) {
            return X;
        } else if(rowProduct[k] == winO || colProduct[k] == winO) {
            return O;
        }
    }

    for(int l = 0; l < 2; l++) {
        if(diagProduct[l] == winX) {
            return X;
        } else if(diagProduct[l] == winO) {
            return O;
        }
    }

    // draw
    return -1;
}

State move(State s, int mark, int i, int j) {
    /* Place mark on tile (i, j). Return the new state of the board. */

    // TODO doesn't check if (i, j) is empty
    s.board.tiles[i][j] = mark;
    s.playerToMove = s.playerToMove == X ? O : X;

    return s;
}

int numPossibleMoves(State s) {
    int n = 0;
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(s.board.tiles[i][j] == EMPTY) {
                n++;
            }
        }
    }

    return n;
}

int** possibleMoves(State s) {
    int n = numPossibleMoves(s);
    int** moves = calloc(n, sizeof(int*));
    for(int i = 0; i < n; i++) {
        moves[i] = calloc(2, sizeof(int));
    }

    int k = 0;

    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(s.board.tiles[i][j] == EMPTY) {
                moves[k][0] = i;
                moves[k][1] = j;
                k++;
            }
        }
    }

    return moves;
}

bool cmpState(State s1, State s2) {
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(s1.board.tiles[i][j] != s2.board.tiles[i][j]) {
                return false;
            }
        }
    }

    return true;
}

bool gameOver(State state) {
    int whoWon = checkWin(state);
    if(whoWon != -1) {
        // someone has won
        return true;
    } else {
        for(int i = 0; i < BOARD_SIZE; i++) {
            for(int j = 0; j < BOARD_SIZE; j++) {
                if(state.board.tiles[i][j] == EMPTY) {
                    return false;
                }
            }
        }

        return true;
    }
}