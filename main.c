#include <stdio.h>
#include <stdint.h>

#include "solver.h"
#include "types.h"
#include "reader.h"
#include "seeer.h"

#define PRINT_STEPS

int main(int argc, char *argv[]) {

    //* Opening file
    if (argc != 2) {
        printf(
            "What te hell are you dooiing???\n"
            "(I need a file with the data of the queens game)\n"
        );
        return -1;
    }

    FILE *file = fopen(argv[1], "r");
    uint32_t size = 0;

    if (file == NULL) {
        fprintf(stderr, "Your stupid file doesn't exist nerd.\n");
        return -1;
    }


    //* Reading file
    if (measureQueensFile(file, &size)) {
        return -1;
    }
    printf("Size: %d\n", size);

    board_t board = createBoard(size);

    fseek(file, 0, SEEK_SET);
    if (readQueensFile(file, &board)) {
        fprintf(stderr, "Reading the board went wrong somehow whoops\n");
        freeBoard(board);
        return -1;
    }

    printBoard(board);

    printf("\n");

    uint8_t solved = 0;
    uint8_t iteration = 0;
    for (; iteration < 7; iteration++) {
        if (solve(board)) {
            solved = 1;
            break;
        }
#ifdef PRINT_STEPS
        printBoard(board);
        printf("\n\n");
#endif
    }

    if (solved == 0) {
        printf("\nCould not solve the board in %d iterations :(\n\n", iteration);
    }
    else {
        printf("\nSolved the board in %d iterations!!\n\n", iteration + 1);
    }

    printBoard(board);

    printf("\n");

    freeBoard(board);

    return 0;
}