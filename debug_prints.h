#ifndef DEBUG_PRINTS_H
#define DEBUG_PRINTS_H


#define DEBUG_PRINTS


#ifdef DEBUG_PRINTS
#define DPRINTF printf
#else
#define DPRINTF(...) // macros
#endif

#endif // DEBUG_PRINTS_H