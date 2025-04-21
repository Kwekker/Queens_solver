#ifndef READER_H
#define READER_H

#include <stdint.h>
#include <stdio.h>

#include "types.h"



int measureQueensFile(FILE *file, uint32_t *size);
int readQueensFile(FILE *file, board_t *board);


#endif // READER_H