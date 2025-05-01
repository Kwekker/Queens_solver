#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "clicker.h"
#include "looker.h"
#include "reader.h"
#include "seeer.h"
#include "solver.h"
#include "types.h"

int main(int argc, char *argv[]) {

    board_t board;
    boardScreenInfo_t screenInfo = {0};

    if (strcmp(argv[1], "auto") == 0) {

        waitForActivation();

        uint32_t size = 0;

        image_t image;
        uint32_t *colors;
        while (1) {
            //* Finding the browser window.
            image = getBrowserWindow();
            size = detectBoard(image, &colors, &screenInfo);

            if (size) break;

            free(image.pixels);
        }


        if (size == 0) {
            return -1;
        }

        if (argc > 2 && strcmp(argv[2], "export") == 0)
            imageToFile(
                "img/eendje.ppm", image.pixels, image.width, image.height
            );

        board = createBoard(size);
        colorBoard(board, colors);

        free(image.pixels);
        free(colors);
    }
    else if (strcmp(argv[1], "file") == 0) {

        //* Opening file
        if (argc < 3) {
            printf(
                "What te hell are you dooiing???\n"
                "(I need a file with the data of the queens game)\n"
                "So like: %s file [filename]\n",
                argv[0]
            );
            return -1;
        }

        FILE *file = fopen(argv[2], "r");
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

        board = createBoard(size);

        fseek(file, 0, SEEK_SET);
        if (readQueensFile(file, &board)) {
            fprintf(stderr, "Reading the board went wrong somehow whoops\n");
            freeBoard(board);
            return -1;
        }
    } else {
        printf("Usage: %s (auto|file) [filename]\n", argv[0]);
    }

    printf("Solving this board:\n");
    printBoard(board, 0);

    printf("\n");

    board = solve(board);

    printf("\nSolved!\n");
    if (screenInfo.offset != 0) {
        printf("Clicking..\n");
        clickSolveBoard(board, screenInfo, atoi(argv[2]));
    }

    printBoard(board, 0);

    printf("\n");

    freeBoard(board);

    return 0;
}