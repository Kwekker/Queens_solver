#include <stdio.h>
#include <stdlib.h>

#include "types.h"


void freeBoard(board_t board);


board_t createBoard(uint32_t size) {
    board_t ret = {0};

    ret.size = size;
    ret.cells = calloc(size * size, sizeof(cell_t));

    cellSet_t *sets = calloc(size * 3, sizeof(cellSet_t));

    // Make sure the three cellSet arrays are consecutive.
    ret.set_arrays[0] = sets;
    ret.set_arrays[1] = sets + size;
    ret.set_arrays[2] = sets + 2 * size;

    printf("Alloc'd the arrays.\n0: %p\n1: %p\n2: %p\n",
        ret.set_arrays[0],
        ret.set_arrays[1],
        ret.set_arrays[2]
    );

    // Populate the set arrays.
    for (uint32_t i = 0; i < size; i++) {

        // Populate the columns with cells.
        ret.columns[i].cells = calloc(size, sizeof(cell_t*));
        ret.columns[i].cellCount = size;
        for (uint32_t j = 0; j < size; j++) {
            ret.columns[i].cells[j] = &ret.cells[j * size + i];
        }

        // Populate the rows with cells.
        ret.rows[i].cells = calloc(size, sizeof(cell_t*));
        ret.rows[i].cellCount = size;
        for (uint32_t j = 0; j < size; j++) {
            ret.rows[i].cells[j] = &ret.cells[i * size + j];
        }

        // We do not yet know the size of the groups,
        // so we cannot populate the groups set.
    }


    // Populate the cell array.
    for (uint32_t i = 0; i < size; i++) {
        for (uint32_t j = 0; j < size; j++) {
            cell_t *cell = &ret.cells[j * size + i];
            cell->x = i;
            cell->y = j;
            cell->column = &ret.columns[i];
            cell->row = &ret.rows[j];
        }
    }

    return ret;
}

// Fucking whole function for freeing one of these beasts.
// I'm starting to understand the whole c++ unique_ptr and shared_ptr business.
// Still a shitty language though.
void freeBoard(board_t board) {
    for (uint8_t s = 0; s < 3; s++) {

        for (uint32_t i = 0; i < board.size; i++) {
            if (board.set_arrays[s][i].cellCount > 0) {
                // Free the array of pointers in the set array.
                free(board.set_arrays[s][i].cells);
            }
        }

    }

    // Free the set arrays.
    free(board.set_arrays[0]);

    // Free the cells.
    free(board.cells);
    // Cells don't contain any heap pointers so they don't need special care.

}


// Moves all populated pointers to the left,
// and sets the cellCount accordingly.
// _ 1 _ _ 2 3 _ 4 5 6 _ 7
// v
// 1 2 3 4 5 6 7
void cleanSet(cellSet_t *set) {
    uint32_t empty_i = 0;
    for (uint32_t i = 0; i < set->cellCount; i++) {
        if (set->cells[i] != NULL) {
            set->cells[empty_i] = set->cells[i];
            empty_i++;
        }
    }
    set->cellCount = empty_i;
}

// Returns the 4 diagonally adjacent cells to a cell.
corners_t getCorners(board_t board, cell_t cell) {
    corners_t corners = {0};

    uint8_t corner_index = 0;
    int8_t x[4] = {-1, 1, -1, 1};
    int8_t y[4] = {-1, -1, 1, 1};

    for (uint8_t i = 0; i < 4; i++) {
        // Check if the corner is in bounds.
        int32_t newX = cell.x + x[i];
        int32_t newY = cell.y + y[i];
        // This has to be a pointer because its index could be
        // outside the bounds of the board's cells array.
        // If it wasn't a pointer, C would try to copy the contents
        // of the array entry into this variable, which could result
        // in a segfault.
        cell_t *corner = &board.cells[newY * board.size + newX];

        if ( // Thank you short-circuiting, my hero.
            newX < 0 || newX >= board.size
            || newY < 0 || newY > board.size
            || corner->type == CELL_CROSSED
        ) continue;

        // Having it be a pointer is also nice for this tbh.
        corners.cells[corner_index++] = corner;
    }

    corners.count = corner_index;

    return corners;
}


void crossCell(cell_t *cell) {
    cell->type = CELL_CROSSED;

    for (uint8_t s = 0; s < 3; s++) {
        cellSet_t *set = cell->sets[s];

        for (uint32_t c = 0; c < set->cellCount; c++) {
            if (set->cells[c] == cell) {
                // Move the last cell of the set into
                // the newly created hole.
                set->cells[c] = set->cells[set->cellCount - 1];
                set->cellCount--;
                break;
            }
        }
    }
}