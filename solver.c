#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "solver.h"
#include "types.h"

//TODO: Use brain and think about the following:
//TODO: Should cells have pointers to their corners within them?


uint8_t checkCellBlocker(cell_t cell);
void setQueen(board_t board, cell_t *cell);
void isolate(cellSet_t *set, cell_t *cell);


void solve(board_t board) {

    // Iterate over all sets of the board.
    for (uint32_t s = 0; s < board.size * 3; s++) {
        cellSet_t set = board.set_arrays[0][s];
        if (set.solved) continue;
        if (set.cellCount == 1) {
            set.solved = 1;
            printf("Found a queen at [%d, %d]\n", set.cells[0]->x, set.cells[0]->y);
            setQueen(board, set.cells[0]);
        }
    }

    // for (uint32_t c = 0; c < board.size; c++) {
    //     if (checkCellBlocker(board.cells[c])) {
    //         printf("Cell [%d, %d] is a blocker\n",
    //             c % board.size, c / board.size
    //         );
    //     };
    // }

}

// todo: Gotta delete the cells
void setQueen(board_t board, cell_t *cell) {
    for (uint8_t i = 0; i < 3; i++) {
        if (cell->sets[i]->cellCount > 1) {
            isolate(cell->sets[i], cell);
        }
    }

    corners_t corners = getCorners(board, *cell);
    for (uint8_t i = 0; i < corners.count; i++) {
        crossCell(corners.cells[i]);
    }

}

// Makes it so that the given cell
// is the only one left in the given set.
void isolate(cellSet_t *set, cell_t *cell) {

    uint32_t cellCount = set->cellCount;
    size_t index = 0;

    // crossCell moves the last cell into the hole created by
    // the crossed cell, so we can just keep crossing index 0.
    // We do need to look out for the queen cell, that one
    // shouldn't get crossed.
    for (uint32_t i = 0; i < cellCount; i++) {
        // This should happen only once
        if (set->cells[index] == cell) index++;
        crossCell(set->cells[index]);

        printf("Crossing cell [%d %d]\n", set->cells[i]->x, set->cells[i]->y);
    }

    if (set.cells[0] != cell) {
        printf("Cell 0 isn't the queen???? huh???\n");
    }


    cell->type = CELL_QUEEN;
    set->cellCount = 1;
    set->solved = 1;
}


uint8_t checkCellBlocker(cell_t cell) {
    return 0;
}