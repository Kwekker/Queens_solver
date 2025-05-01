#include <X11/X.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "clicker.h"
#include "looker.h"
#include "reader.h"
#include "seeer.h"
#include "solver.h"
#include "types.h"

int main(int argc, char *argv[]) {

    struct option long_options[] = {
        {"file", required_argument, 0, 'f'},
        {"window", required_argument, 0, 'w'},
        {"delay", no_argument, 0, 'd'},
        {"detached-colors", no_argument, 0, 'c'},
        {"no-solve", no_argument, 0, 'n'},
        {0, 0, 0, 0}
    };

    uint8_t detached_colors = 0;
    uint32_t click_delay = 800;
    Window input_window = 0;
    uint8_t dont_click_solve = 0;
    FILE *file = NULL;

    while (1) {
        int option_index = 0;

        int opt = getopt_long(argc, argv, "cdnw:f:", long_options, &option_index);
        if (opt == -1) {
            printf("No args left!\n");
            break;
        }

        switch (opt) {

            case 'c':
                detached_colors = 1;
                break;
            case 'n':
                dont_click_solve = 1;
                break;

            case 'f':
                file = fopen(optarg, "r");
                if (file == NULL) {
                    fprintf(stderr, "File %s not found.\n", optarg);
                    return -1;
                }
                break;

            case 'd':
                click_delay = strtol(optarg, NULL, 0);
                if (click_delay == 0) {
                    fprintf(stderr,
                        "Could not parse delay number."
                        "A delay of 0 is unsupported (use 1)\n"
                    );
                }
                break;

            case 'w':
                input_window = strtol(optarg, NULL, 0);
                if (input_window == 0) {
                    fprintf(stderr, "Could not parse window number.\n");
                }
                break;



            default:
                fprintf(stderr, "getopt returned character code 0x%x??\n", opt);
        }
    }

    board_t board;
    boardScreenInfo_t screenInfo = {0};

    // Automatic board detection
    if (file == NULL) {
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

        if (argc > 2 && strcmp(argv[2], "export") == 0) {
            imageToFile(
                "img/eendje.ppm", image.pixels, image.width, image.height
            );
        }


        // Create and color the board.
        board = createBoard(size);
        colorBoard(board, colors);
        printf("Solving this board:\n");
        printBoard(board, 0);
        printf("\n");

        free(image.pixels);
        free(colors);

        // Solve the board.
        board = solve(board);

        printf("\nSolved!\n");
        if (!dont_click_solve) {
            printf("Clicking..\n");
            clickSolveBoard(board, screenInfo, click_delay);
        }

        printBoard(board, 0);

        printf("\n");

        freeBoard(board);
    }
    // File reading
    else {

        uint32_t size = 0;

        if (file == NULL) {
            fprintf(stderr, "Your stupid file doesn't exist nerd.\n");
            return -1;
        }

        //* Reading file
        if (measureQueensFile(file, &size)) {
            fprintf(stderr, "Something went wrong while measuring the file.\n");
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
    }
}