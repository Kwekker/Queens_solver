#ifndef SOLVER_H
#define SOLVER_H

#include "types.h"


#define MAX_ATTEMPTS 7

// #define PRINT_INTERMEDIATE
// #define PRINT_LOGS

// Use this function like this:
// myBoard = solve(myBoard);
// Pointers in myBoard might change completely.
// It will free the previous pointers if that is the case.
// You do still need to free the returned board when you're done with it.
board_t solve(board_t board);
board_t bruteForce(board_t board, cellSet_t *group, uint32_t depth);


#endif