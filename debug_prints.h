#ifndef DEBUG_PRINTS_H
#define DEBUG_PRINTS_H


#define DEBUG_PRINTS

#define PRINT_NORMAL 1
#define PRINT_BLOCKERS 0
#define PRINT_CROSSINGS 0
#define PRINT_SEEING 0

#define DEBUG_PRINT_MODE PRINT_NORMAL


// #define DPRINTF(cformat, ...) if(DEBUG_PRINT_MODE) {fprintf(stderr, (cformat), __VA_ARGS__);}

#define DPRINTF(...) if(DEBUG_PRINT_MODE) fprintf(stderr, __VA_ARGS__)



#endif // DEBUG_PRINTS_H