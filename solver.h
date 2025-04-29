#ifndef SOLVER_H
#define SOLVER_H

#include "types.h"


#define MAX_ATTEMPTS 7

// #define PRINT_INTERMEDIATE
// #define PRINT_LOGS

uint8_t solve(board_t board);
board_t bruteForce(board_t board, cellSet_t *group, uint32_t depth);


#endif