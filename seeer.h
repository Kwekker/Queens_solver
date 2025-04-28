#ifndef SEEER_H_
#define SEEER_H_

#include "looker.h"


// The amount of pixels to not take into account at the borders of the window.
// This should *alwayys* be more than like 16
#define WINDOW_BORDER_MARGIN 32

// The maximum distance the detected points may deviate
// from the calculated median, to be counted as a valid point.
#define MAX_OFFSET_ERROR 5


// The threshold below which a pixel is considered black.
#define BLACK_THRESHOLD 10


#if WINDOW_BORDER_MARGIN < 16
    #error FUCK!!! Your WINDOW_BORDER_MARGIN is too SMALL!!!
#endif


// Returns the size of the board, or 0 when it didn't detect one.
uint32_t detectBoard(image_t image, uint32_t **board, int crossingOffset);


#endif




// int main(int argc, char *argv[]) {
//     Display *display = XOpenDisplay(NULL);
//     Window root = DefaultRootWindow(display);

//     Window viv = findBrowser(display, root, 0);

//     // printProperties(display, viv);

//     screenshotToFile(display, "screenshot.ppm", viv);


//     return 0;
// }

