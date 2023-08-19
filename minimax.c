#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "state.h"

#define MIN 0
#define MAX 1

typedef struct Node {
    State state;
    int score;
    int playerType;
    int numChildren;
    struct Node** children;
} Node;

typedef struct Tree {
    Node* root;
} Tree;

Node* createNode(int score, int playerType) {
    Node* node = malloc(sizeof(Node));
    node->score = score;
    node->playerType = playerType;
    node->numChildren = 0;

    return node;
}

int generateSubTree(Node* node, int playerMark) {
    /*
    Generate the subtree below node and return the score 
    obtained after backpropagation.
    */
    
    // TODO use gameOver

    int hasAnyoneWon = checkWin(node->state);
    if(hasAnyoneWon != -1) {
        // someone has won
        node->score = hasAnyoneWon == playerMark ? WIN : LOSS;
        return node->score;
    }

    int numMoves = numPossibleMoves(node->state);

    if(numMoves == 0) {
        // game has finished
        node->score = DRAW;
        return node->score;
    }

    int** moves = possibleMoves(node->state);

    node->numChildren = numMoves;
    node->children = calloc(numMoves, sizeof(Node));

    // switch player type for children
    int childrenPlayerType = (node->playerType + 1) % 2;
    int childScore, nodeScore, markToPlay;
    int opponentMark = playerMark == X ? O : X;

    for(int i = 0; i < numMoves; i++) {
        node->children[i] = createNode(0, childrenPlayerType);

        // decide what mark (X, O) to play this turn
        markToPlay = node->playerType == MAX ? playerMark : opponentMark;
        node->children[i]->state = move(node->state, markToPlay, moves[i][0], moves[i][1]);
        childScore = generateSubTree(node->children[i], playerMark);

        if(i == 0) {
            nodeScore = childScore;
        } else if(node->playerType == MAX) {
            // score is the maximum of all the scores of children
            nodeScore = childScore > nodeScore ? childScore : nodeScore;
        } else {
            // score is the minimum of all the scores of children
            nodeScore = childScore > nodeScore ? nodeScore : childScore;
        }
    }

    for(int i = 0; i < numMoves; i++) {
        free(moves[i]);
    }
    free(moves);

    node->score = nodeScore;
    return node->score;
}

Tree generateTree(int playerMark) {
    int rootPlayerType = playerMark == X ? MAX : MIN;
    Node* root = createNode(0, rootPlayerType);
    root->state = initState();
    generateSubTree(root, playerMark);
    Tree t;
    t.root = root;
    return t;
}

void freeNode(Node* node) {
    for(int i = 0; i < node->numChildren; i++) {
        freeNode(node->children[i]);
    }
    free(node);
}

int* moveSeparatingStates(State parentState, State childState) {
    /*
    Return the move that separates the state of a parent node 
    from that of a child node.

    The first element of the array is the mark that was played.

    Return NULL if the states are the same.
    */

    int move[3];

    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(parentState.board.tiles[i][j] != childState.board.tiles[i][j]) {
                move[0] = childState.board.tiles[i][j];
                move[1] = i;
                move[2] = j;
                return move;
            }
        }
    }

    return NULL;
}

Node* findStateNode(State* states, Tree gameTree, int numTurns) {
    /* Find the node corresponding to the current game state using the history of states.  */

    // TODO check correctness

    Node* node = gameTree.root;

    for(int i = 0; i < numTurns; i++) {
        for(int j = 0; j < node->numChildren; j++) {
            if(cmpState(states[i], node->children[j]->state)) {
                node = node->children[j];
                break;
            }
        }
    }

    return node;
}

State takeTurnPlayer(Tree gameTree, State gameState, int playerMark) {
    /* Return the new game state after the player takes a turn. */

    int x, y;
    
    do {
    printf("Coordinate to place mark: ");
    scanf("%d %d", &x, &y);
    } while((x >= 0 && x <= BOARD_SIZE) && (y >= 0 && y <= BOARD_SIZE) && gameState.board.tiles[x][y] != EMPTY);

    return move(gameState, playerMark, x, y);
}

State takeTurnComputer(Tree gameTree, State states[9], int numTurns) {
    /* Return the new game state after the computer takes a turn. */

    Node* currentNode = findStateNode(states, gameTree, numTurns);
    
    for(int i = 0; i < currentNode->numChildren; i++) {
        if(currentNode->score == currentNode->children[i]->score) {
            return currentNode->children[i]->state;
        }
    }
}

void drawBoard(State gameState) {
    char toDraw;
    for(int i = 0; i < BOARD_SIZE; i++) {
        printf("|");
        for(int j = 0; j < BOARD_SIZE; j++) {
            switch(gameState.board.tiles[i][j]) {
                case X:
                    toDraw = 'X';
                    break;
                case O:
                    toDraw = 'O';
                    break;
                case EMPTY:
                    toDraw = ' ';
                    break;
            }
            printf("%c|", toDraw);
        }
        printf("\n");
    }
}

void game() {

    // TODO allow player to pick mark
    int playerMark = X;
    Tree gameTree = generateTree(X);

    // keep track of what turn is being played
    int numTurn = 1;
    int result;
    State states[9];

    State gameState = initState();
    State newState;

    do {
        if(numTurn % 2 == 0) {
            if(playerMark == O) {
                drawBoard(gameState);
                newState = takeTurnPlayer(gameTree, gameState, playerMark);
            } else {
                newState = takeTurnComputer(gameTree, states, numTurn);
            }
        } else {
            if(playerMark == X) {
                drawBoard(gameState);
                newState = takeTurnPlayer(gameTree, gameState, playerMark);
            } else {
                newState = takeTurnComputer(gameTree, states, numTurn);
            }
        }

        states[numTurn-1] = newState;
        gameState = newState;
        numTurn++;

    } while(!(result = gameOver(gameState)));

    printf("\n\nGame Over.\n\n\n");
    switch(checkWin(gameState)) {
        case X:
            if(playerMark == X) {
                printf("You Won!\n\n");
            } else {
                printf("Computer Won!\n\n");
            }
            break;
        case O:
            if(playerMark == O) {
                printf("You Won!\n\n");
            } else {
                printf("Computer Won!\n\n");
            }
            break;
        case -1:
            // draw
            printf("It's a draw!\n\n");
            break;
    }
    drawBoard(gameState);

    // release memory
    freeNode(gameTree.root);
}

int main(int argc, char const *argv[]) {
    game();
    return 0;
}
