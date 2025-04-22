#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "solver.h"
#include "types.h"

#define GREEN "\x1b[33m"
#define NONE "\x1b[0m"

//TODO: Use brain and think about the following:
//TODO: Should cells have pointers to their corners within them?


static uint8_t checkCellBlocker(board_t board, cell_t *cell);
static void setQueen(board_t board, cell_t *cell);
static void isolate(cellSet_t *set, cell_t *cell);
static uint8_t markCell(
    cell_t *potentialBlocker, cell_t *markCell,
    cellSet_t **affectedSets, size_t *affectedSet_i
);



//! Delete this
board_t gBoard;


void solve(board_t board) {

    gBoard = board;

    // Iterate over all sets of the board.
    for (uint32_t s = 0; s < board.size * 3; s++) {
        cellSet_t set = board.set_arrays[0][s];
        if (set.solved == 1) continue;
        if (set.cellCount == 1) {
            set.solved = 1;
            printf("Found a queen at [%d, %d]\n", set.cells[0]->x, set.cells[0]->y);
            setQueen(board, set.cells[0]);
        }
    }

    for (uint32_t j = 0; j < board.size; j++) {
        for (uint32_t i = 0; i < board.size; i++) {
            cell_t *cell = &board.cells[i + j*board.size];

            if (cell->type == CELL_CROSSED) continue;
            if (checkCellBlocker(board, cell)) {
                crossCell(cell);
            }
        }
    }

}


void setQueen(board_t board, cell_t *cell) {

    for (uint8_t i = 0; i < 3; i++) {
        if (cell->sets[i]->cellCount > 1) {
            isolate(cell->sets[i], cell);
        }
        cell->sets[i]->solved = 1;
    }

    corners_t corners = getCorners(board, *cell);
    for (uint8_t i = 0; i < corners.count; i++) {
        crossCell(corners.cells[i]);
    }

    cell->type = CELL_QUEEN;
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
        if (set->cells[index] == cell) {
            index++;
            continue;
        }

        crossCell(set->cells[index]);
    }

    if (set->cells[0] != cell) {
        printf("Cell 0 isn't the queen???? huh???\n");
        printf("It's fuckin %p where the queen is %p\n", set->cells[0], cell);
    }

    set->cellCount = 1;
    set->solved = 1;
}


// Check if this cell being a queen would completely
// empty out a set of cells.
// (Like, it blocks a group completely or something.)
uint8_t checkCellBlocker(board_t board, cell_t *cell) {

    // Count amount of sets this cell is going to affect.
    size_t affectedSetCount = 3 * (
        cell->column->cellCount + cell->row->cellCount + cell->group->cellCount
        - 3 // We ignore the cell we're checking in the iterations,
            // so we don't count it.
    ) + 4; // Add 4 for the four corners.

    // Allocate an array of that amount of pointers.
    cellSet_t **affectedSets = malloc(affectedSetCount * sizeof(cellSet_t*));
    size_t affectedSet_i = 0;

    uint8_t isBlocker = 0;

    // Iterate over every affected set.
    for (uint32_t s = 0; s < 3; s++) {
        for (uint32_t c = 0; c < cell->sets[s]->cellCount; c++) {
            cell_t *mCell = cell->sets[s]->cells[c];
            if (mCell == cell || mCell->type == CELL_MARKED)
                continue;
            mCell->type = CELL_MARKED;

            if (markCell(cell, mCell, affectedSets, &affectedSet_i)) {
                isBlocker = 1;
                goto break_all;
            }
        }
    }

    // Iterate over corners.
    corners_t corners = getCorners(board, *cell);
    for (uint8_t i = 0; i < corners.count; i++) {
        if (markCell(cell, corners.cells[i], affectedSets, &affectedSet_i)) {
            isBlocker = 1;
            break;
        }
    }


    break_all:

    // Unmark cells
    for (uint32_t s = 0; s < 3; s++) {
        for (uint32_t c = 0; c < cell->sets[s]->cellCount; c++) {
            cell_t *markCell = cell->sets[s]->cells[c];
            if (markCell->type == CELL_MARKED)
                markCell->type = CELL_EMPTY;
        }
    }

    // Set the variable of all the affected sets back to 0.
    for (size_t i = 0; i < affectedSet_i; i++) {
        affectedSets[i]->variable = 0;
    }

    free(affectedSets);

    return isBlocker;
}


uint8_t markCell(
    cell_t *potentialBlocker, cell_t *markCell,
    cellSet_t **affectedSets, size_t *affectedSet_i
) {
    markCell->type = CELL_MARKED;

    for (uint32_t mark_s = 0; mark_s < 3; mark_s++) {
        cellSet_t *markSet = markCell->sets[mark_s];
        if (inSet(markSet, potentialBlocker)) continue;

        // We use the cool variable field for this
        // (very useful)
        markSet->variable++;
        affectedSets[*affectedSet_i] = markSet;
        (*affectedSet_i)++;

        if (markSet->cellCount - markSet->variable <= 0) {
            visuPrompt(gBoard, potentialBlocker, markCell, markSet);
            return 1;
        }
    }

    return 0;
}