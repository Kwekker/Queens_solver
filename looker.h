#ifndef LOOKER_H_
#define LOOKER_H_

#include <X11/X.h>
#include <X11/Xlib.h>

#include <stdint.h>


typedef struct {
    union {
        struct {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };
        uint8_t rgb[3];
    };
} pixel_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    pixel_t *pixels;
} image_t;


Window findBrowser(Display *display, Window root, int depth);
image_t getBrowserWindow(void);
void imageToFile(
    const char *fileName, pixel_t *image, uint32_t width, uint32_t height
);

#endif




// int main(int argc, char *argv[]) {
//     Display *display = XOpenDisplay(NULL);
//     Window root = DefaultRootWindow(display);

//     Window viv = findBrowser(display, root, 0);

//     // printProperties(display, viv);

//     screenshotToFile(display, "screenshot.ppm", viv);


//     return 0;
// }

