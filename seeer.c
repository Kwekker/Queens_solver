#include <X11/X.h>
#include <X11/Xlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// gcc -o seeer seeer.c -Wall -lX11

int main(int argc, char *argv[]) {
    Display *display = XOpenDisplay(NULL);
    Window root = DefaultRootWindow(display);

    Window interested;
    Window idc1, idc2;

    Window *children;
    unsigned int nChildren = 0;

    XQueryTree(display, root, &idc1, &idc2, &children, &nChildren);

    int skip = atoi(argv[1]);

    for (unsigned int i = 0; i < nChildren; i++) {
        XWindowAttributes attrs;
        XGetWindowAttributes(display, children[i], &attrs);

        if (attrs.class == InputOutput) {
            if (skip) {
                skip--;
                continue;
            }
            char *name;
            XFetchName(display, children[i], &name);
            interested = children[i];
            fprintf(stderr, "Found %s\n", name);
            free(name);
            break;
        }
    }

    XWindowAttributes gwa;

    XGetWindowAttributes(display, interested, &gwa);
    int width = gwa.width;
    int height = gwa.height;

    fprintf(stderr, "I got here haha\n");

    XImage *image =
    XGetImage(display, interested, 0, 0, width, height, AllPlanes, ZPixmap);

    unsigned char *array = malloc(width * height * 3);

    unsigned long red_mask = image->red_mask;
    unsigned long green_mask = image->green_mask;
    unsigned long blue_mask = image->blue_mask;

    printf("P6\n%d\n%d\n255\n", width, height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned long pixel = image->f.get_pixel(image, x, y);

            unsigned char blue = pixel & blue_mask;
            unsigned char green = (pixel & green_mask) >> 8;
            unsigned char red = (pixel & red_mask) >> 16;

            unsigned char rgb[3] = {red, green, blue};

            array[(x + width * y) * 3] = red;
            array[(x + width * y) * 3 + 1] = green;
            array[(x + width * y) * 3 + 2] = blue;

            fwrite(rgb, 1, 3, stdout);
        }
        // printf("\n");
    }

    free(array);

    return 0;
}