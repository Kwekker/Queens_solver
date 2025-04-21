#include <stdio.h>
#include <stdint.h>

#include "solver.h"
#include "types.h"
#include "reader.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf(
            "What te hell are you dooiing???\n"
            "(I need a file with the data of the queens game)\n"
        );
        return -1;
    }

    board_t board = createBoard(8);

    FILE *file = fopen(argv[1], "r");
    uint32_t size = 0;

    if (measureQueensFile(file, &size)) {
        return -1;
    }
    printf("Size: %d\n", size);

    fseek(file, 0, SEEK_SET);
    if (readQueensFile(file, &board)) {
        fprintf(stderr, "Reading the board went wrong somehow whoops\n");
        return -1;
    }

    for (uint32_t j = 0; j < board.size; j++) {
        for (uint32_t i = 0; i < board.size; i++) {
            printf("%d", board.cells[j * board.size + i].color);
        }
        printf("\n");
    }

    for (uint32_t i = 0; i < board.size; i++) {
        printf("Group %u size is %zu\n", i, board.groups[i].cellCount);
    }

    solve(board);

    printf("Cols: ");
    for (uint32_t i = 0; i < board.size; i++) {
        printf("%2zu ", board.columns[i].cellCount);
    }
    printf("\nRows: ");
    for (uint32_t i = 0; i < board.size; i++) {
        printf("%2zu ", board.columns[i].cellCount);
    }
    printf("\nGrps: ");
    for (uint32_t i = 0; i < board.size; i++) {
        printf("%2zu ", board.columns[i].cellCount);
    }
    printf("\n");

    freeBoard(board);

    return 0;
}