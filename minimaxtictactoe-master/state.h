#ifndef STATE_H
#define STATE_H

#define BOARD_SIZE 3

#define EMPTY 1
#define X 2
#define O 3

#define WIN 1
#define LOSS -1
#define DRAW 0
#define NOT_OVER 2

typedef struct Board {
    int tiles[3][3];
} Board;

typedef struct State {
    Board board;
    int playerToMove;
} State;

Board initBoard();
State initState();
int checkWin(State s);
State move(State s, int mark, int i, int j);
int numPossibleMoves(State s);
int** possibleMoves(State s);
bool cmpState(State s1, State s2);
bool gameOver(State state);

#endif