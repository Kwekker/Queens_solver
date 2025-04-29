#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "solver.h"
#include "types.h"

#include "debug_prints.h"

#define GREEN "\x1b[33m"
#define NONE "\x1b[0m"

// #define PRINT_STEPS
// #define PRINT_INTERMEDIATE



static uint8_t checkCellBlocker(board_t board, cell_t *cell);
static void setQueen(board_t board, cell_t *cell);
static void isolate(cellSet_t *set, cell_t *cell);
static uint8_t markCell(
    cell_t *potentialBlocker, cell_t *markCell,
    cellSet_t **affectedSets, size_t *affectedSet_i
);
static void checkColRowRedundancy(board_t board);


//! Delete this
board_t gBoard;


board_t solve(board_t board) {

    uint32_t prevTotalCellCount = -1;
    [[maybe_unused]]
    uint32_t iteration = 0;
    while (1) {
        DPRINTF("Iteration %d\n", iteration);
        iteration++;

        gBoard = board;

        // Iterate over all sets of the board.
        for (uint32_t s = 0; s < board.size * 3; s++) {
            cellSet_t set = board.set_arrays[0][s];
            if (set.solved == 1) continue;
            if (set.cellCount == 1) {
                set.solved = 1;
                DPRINTF("Found queen at [%d, %d]\n",
                    set.cells[0]->x, set.cells[0]->y
                );
                setQueen(board, set.cells[0]);
            }
        }

#ifdef PRINT_INTERMEDIATE
        printf("Intermediate board:\n");
        printBoard(board, 0);
        printf("\n");
#endif

        for (uint32_t j = 0; j < board.size; j++) {
            for (uint32_t i = 0; i < board.size; i++) {
                cell_t *cell = &board.cells[i + j*board.size];

                if (cell->type == CELL_CROSSED) continue;
                if (checkCellBlocker(board, cell)) {
                    crossCell(cell);
                }
            }
        }


        uint32_t totalCellCount = 0;
        for (uint32_t i = 0; i < board.size; i++) {
            totalCellCount += board.groups[i].cellCount;
        }

        if (
            totalCellCount == prevTotalCellCount
            && totalCellCount > board.size
        ) {
            DPRINTF(
                "The board is not solvable using quick methods. "
                "Bruteforcing time!\n"
            );
            board_t solved = bruteForce(board, &board.groups[0], 0);
            freeBoard(board);
            return solved;
        }

        prevTotalCellCount = totalCellCount;

#ifdef PRINT_STEPS
        printBoard(board, 0);
        printf("\n\n");
#endif

        // Check if solved.
        if (totalCellCount != board.size) continue;

        // Solved!
        return board;
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
        fprintf(stderr, "Cell 0 isn't the queen???? huh???\n");
        fprintf(stderr, "It's fuckin %p where the queen is %p\n",
            set->cells[0], cell
        );
    }

    set->cellCount = 1;
    set->solved = 1;
}


#undef DEBUG_PRINT_MODE
#define DEBUG_PRINT_MODE PRINT_BLOCKERS

#if DEBUG_PRINT_MODE
#error fuck!!!
#endif


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
            if (mCell == cell) {
                continue;
            }

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

    // Unmark corner cells
    for (uint8_t i = 0; i < corners.count; i++)
        if(corners.cells[i]->type == CELL_MARKED)
            corners.cells[i]->type = CELL_EMPTY;

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

void checkColRowGroupRedundancy(board_t board) {

    uint32_t groupCounts[2][board.size];

    // Iterate over the columns and rows
    for (uint32_t s = 0; s < 2; s++) {
        memset(groupCounts[s], 0, board.size * sizeof(uint32_t));

        // For every column/row
        for (uint32_t set_i = 0; set_i < board.size; set_i++) {

            cellSet_t *set = &board.set_arrays[s][set_i];

            // For every cell within that column/row,
            // count the amount of different groups.
            for (uint32_t c = 0; c < set->cellCount; c++) {
                cell_t *cell = set->cells[c];

                if (cell->group->variable) continue;
                cell->group->variable = 1;
                groupCounts[s][set_i]++;
            }
            // Reset the variable
            for (uint32_t c = 0; c < set->cellCount; c++) {
                set->cells[c]->group->variable = 0;
            }
        }
    }

    printf("Col counts: \n");
    for (uint32_t i = 0; i < board.size; i++) {
        printf("%2d ", groupCounts[0][i]);
    }
    printf("\nRow counts: \n");
    for (uint32_t i = 0; i < board.size; i++) {
        printf("%2d ", groupCounts[1][i]);
    }
    printf("\n");
}



void checkGroupRedundancy(board_t board) {

}


void checkColRowRedundancy(board_t board) {

}



uint8_t markCell(
    cell_t *potentialBlocker, cell_t *markCell,
    cellSet_t **affectedSets, size_t *affectedSet_i
) {
    // Don't doubly mark cells.
    if (markCell->type == CELL_MARKED) return 0;
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
#if DEBUG_PRINT_MODE
            visuPrompt(gBoard, potentialBlocker, markCell, markSet);
#endif
            return 1;
        }
    }

    return 0;
}




board_t bruteForce(board_t board, cellSet_t *group, uint32_t depth) {
    for (uint32_t c = 0; c < group->cellCount; c++) {
        board_t copy = copyBoard(board);

        // Get cell from original board.
        cell_t *cell = group->cells[c];
        // Translate that to a cell from the copied board.
        cell = &copy.cells[cell->x + copy.size * cell->y];

        for (uint8_t t = 0; t < depth; t++) DPRINTF("\t");
        DPRINTF("%2d: Trying queen at [%d, %d]\n",
            group->identifier, cell->x, cell->y
        );

        if (checkCellBlocker(copy, cell)) {
            for (uint8_t t = 0; t < depth; t++) DPRINTF("\t");
            DPRINTF("It a blocker..\n");
            freeBoard(copy);
            continue;
        }

        setQueen(copy, cell);

        if (checkBoard(copy)) {
            for (uint8_t t = 0; t < depth; t++) DPRINTF("\t");
            DPRINTF("Bad idea..\n");
            freeBoard(copy);
            continue;
        }

#if DEBUG_PRINT_MODE
        printBoard(copy, depth);
#endif

        // If we passed the check and this is the last group,
        // we have solved the board.
        if (depth + 1 == board.size) return copy;


        board_t ret = bruteForce(
            copy, &copy.groups[group->identifier + 1], depth + 1
        );

        // Return the board if the next recursion layer solved it.
        if (ret.size) {
            freeBoard(copy);
            return ret;
        }

        freeBoard(copy);
    }

    return (board_t){.size = 0};
}






#undef DEBUG_PRINT_MODE
#define DEBUG_PRINT_MODE PRINT_NORMAL
