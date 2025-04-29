#ifndef TYPES_H
#define TYPES_H


#include <stdint.h>
#include <stdlib.h>
 


typedef struct cellSet_struct cellSet_t;

#define CELL_EMPTY 0
#define CELL_CROSSED 1
#define CELL_QUEEN 2
#define CELL_MARKED 3

// Every cell has 3 properties: x, y, and color.
// These properties each put it in a column, row, and group, respectively.
// These three things are "sets".
typedef struct {
    uint32_t color;
    uint32_t x;
    uint32_t y;
    uint8_t type;

    // Useful variable used for various things.
    // Always set back to 0 at the end of a function using it.
    uint16_t variable;
    
    // God fuck I love C's anonymous structs and unions.
    union {
        struct {
            cellSet_t *column;
            cellSet_t *row;
            cellSet_t *group;
        };
        cellSet_t *sets[3];
    };
    
} cell_t;


// Columns, rows, and groups (cells of the same color) are all identified as "sets".
// This is because they are basically identical in function while solving;
// every set may only have one queen. Every cell is one set of each type.
struct cellSet_struct{
    uint32_t identifier;

    // Pointer to the array of pointers to cells that are in this group
    cell_t **cells;
    int32_t cellCount;

    uint8_t solved;

    // Handy variable to keep around.
    // Used by functions for various things.
    // Assumed to always be set back to 0.
    int16_t variable;
};


typedef struct {

    // Size is the amount of columns, rows, groups, and queens,
    // which are all equal.
    uint32_t size;

    // The amount of cells on a board is the size squared.
    cell_t *cells;

    union {
        struct {
            cellSet_t *columns;
            cellSet_t *rows;
            cellSet_t *groups;
        };
        // This name tries to make it very explicit that 
        // this is an array of pointers to *arrays* of sets.
        cellSet_t *set_arrays[3];
    };

} board_t;


typedef struct {
    cell_t *cells[4];
    uint8_t count;
} corners_t;


board_t createBoard(uint32_t size);
void freeBoard(board_t board);
board_t copyBoard(board_t board);
corners_t getCorners(board_t board, cell_t cell);
void crossCell(cell_t *cell);
uint8_t inSet(cellSet_t *set, cell_t* cell);
void colorBoard(board_t board, uint32_t *colors);

uint8_t checkBoard(board_t board);
void visuPrompt(board_t board, cell_t *cell, cell_t *markCell, cellSet_t *markSet);


void printBoard(board_t board, uint32_t indentation);
void printBoardVars(board_t board);


#endif