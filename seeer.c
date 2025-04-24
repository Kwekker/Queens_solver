#include <X11/X.h>
#include <X11/Xlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// gcc -o seeer seeer.c -Wall -lX11

#define PRINT_PROPERTIES_IN_TREE


void printKids(Display *display, Window *kids, uint32_t kidCount, uint32_t depth) {

    for (uint32_t i = 0; i < kidCount; i++) {
        XWindowAttributes attrs;
        XGetWindowAttributes(display, kids[i], &attrs);

        for (uint32_t d = 0; d < depth; d++) fprintf(stderr, "\t");
        fprintf(stderr, "Kid \x1b[33m%d\x1b[0m: ", i);


        // Name
        char *name;
        XFetchName(display, kids[i], &name);

        fprintf(stderr, "\"\x1b[34m%s\x1b[0m\" (%lx) ", name, kids[i]);
        if (attrs.class == InputOutput) fprintf(stderr, "[\x1b[32mINOUT\x1b[0m] ");
        free(name);

#ifdef PRINT_PROPERTIES_IN_TREE
        int propertiesCount;
        Atom *properties = XListProperties(display, kids[i], &propertiesCount);
        fprintf(stderr, "properties:\n");
        for (int p = 0; p < propertiesCount; p++) {
            for (uint32_t d = 0; d < depth + 1; d++) fprintf(stderr, "\t");
            char *atomName = XGetAtomName(display, properties[p]);
            fprintf(stderr, "[%s]\n", atomName);
        }
#endif

        Window idc1, idc2;
        Window *kidsKids;
        uint32_t kidsKidCount = 0;
        XQueryTree(display, kids[i], &idc1, &idc2, &kidsKids, &kidsKidCount);

        if (kidsKidCount > 0) {
#ifdef PRINT_PROPERTIES_IN_TREE
            for (uint32_t d = 0; d < depth; d++) fprintf(stderr, "\t");
#endif
            printf("has %d kids:\n", kidsKidCount);
            printKids(display, kidsKids, kidsKidCount, depth + 1);
        }
#ifndef PRINT_PROPERTIES_IN_TREE
        else printf("\n");
#endif

        XFree(kidsKids);

    }
}


Window findVivaldi(Display *display, Window root) {

    Window idc1, idc2;
    Window *children;
    uint32_t kidCount = 0;
    XQueryTree(display, root, &idc1, &idc2, &children, &kidCount);

    for (uint32_t i = 0; i < kidCount; i++) {
        XWindowAttributes attrs;
        XGetWindowAttributes(display, children[i], &attrs);

        // Name
        char *name;
        XFetchName(display, children[i], &name);
        if (
            attrs.class == InputOutput
            && name != NULL
            && strcmp(name, "vivaldi-stable") == 0
        ) {
            fprintf(stderr, "Found vivaldi!!\n");
            XFree(name);
            return children[i];
        }
        XFree(name);

    }

    XFree(children);
    return root;
}


void screenshotToFile(Display *display, char *fileName, Window window) {
    XWindowAttributes gwa;
    XGetWindowAttributes(display, window, &gwa);
    int width = gwa.width;
    int height = gwa.height;

    if (gwa.class == InputOnly) {
        printf("this fucker is InputOnly!\n");
        return;
    }

    printf("getting image of [%d, %d]\n", width, height);
    XImage *image =
        XGetImage(display, window, 0, 0, width, height, AllPlanes, ZPixmap);

    if (image == NULL) {
        printf("Window is cringe\n");
        return;
    }
    printf("got image\n");


    FILE *file = fopen(fileName, "w");

    unsigned char *array = malloc(width * height * 3);

    unsigned long red_mask = image->red_mask;
    unsigned long green_mask = image->green_mask;
    unsigned long blue_mask = image->blue_mask;

    fprintf(file, "P6\n%d\n%d\n255\n", width, height);

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

            fwrite(rgb, 1, 3, file);
        }
    }

    fclose(file);

    free(array);
}


void printProperties(Display *display, Window window) {
    int propertiesCount;
    Atom *properties = XListProperties(display, window, &propertiesCount);
    fprintf(stderr, "properties:\n");
    for (int p = 0; p < propertiesCount; p++) {
        char *atomName = XGetAtomName(display, properties[p]);

        Atom type;
        int format;
        unsigned long nitems;
        unsigned long remaining;
        unsigned char *data;

        XGetWindowProperty(
            display,
            window,
            properties[p],
            0, 32, 0,
            AnyPropertyType,
            &type,
            &format,
            &nitems,
            &remaining,
            &data
        );

        fprintf(stderr, "[%s]\n\t", atomName);
        for (int i = 0; i < 32; i++) {
            fprintf(stderr, "%2x ", data[i]);
        }
        fprintf(stderr, "\n\t");
        for (int i = 0; i < 32; i++) {
            if(data[i] > ' ' && data[i] < 0x7f)
                fprintf(stderr, " %c ", data[i]);
            else if (data[i] == ' ')
                fprintf(stderr, " \x1b[31m_\x1b[0m ");
            else
                fprintf(stderr, "   ");
        }
        fprintf(stderr, "\n%lu, %d, %lu, %lu\n", type, format, nitems, remaining);

    }
}


uint8_t isBrowser(Display *display, Window window) {

    Atom atom = XInternAtom(display, "WM_WINDOW_ROLE", 1);
    if (atom == None) return 0;

    Atom type;
    int format;
    unsigned long nitems;
    unsigned long remaining;
    unsigned char *data;

    XGetWindowProperty(
        display,
        window,
        atom,
        0, 32, 0,
        AnyPropertyType,
        &type,
        &format,
        &nitems,
        &remaining,
        &data
    );
    if (nitems != 7) return 0;
    return memcmp(data, "browser", 7) == 0;

}

Window findBrowser(Display *display, Window root, int depth) {
    Window idc1, idc2;
    Window *kids;
    uint32_t kidCount = 0;
    XQueryTree(display, root, &idc1, &idc2, &kids, &kidCount);

    for (uint32_t d = 0; d < depth; d++) printf("\t");
    printf("has %d kids\n", kidCount);

    for (uint32_t i = 0; i < kidCount; i++) {

        for (uint32_t d = 0; d < depth; d++) printf("\t");
        printf("kid %d [%lx]: ", i, kids[i]);
        if (isBrowser(display, kids[i])) {
            printf("it a brows!!\n");
            return kids[i];
        }
        printf("Not a brows :( \n");


        Window potentialBrowser = findBrowser(display, kids[i], depth + 1);
        if (potentialBrowser != 0) return potentialBrowser;
    }

    XFree(kids);
    return 0;
}



int main(int argc, char *argv[]) {
    setbuf(stderr, NULL);
    setbuf(stdout, NULL);
    Display *display = XOpenDisplay(NULL);
    Window root = DefaultRootWindow(display);

    Window viv = findBrowser(display, root, 0);

    // printProperties(display, viv);

    screenshotToFile(display, "screenshot.ppm", viv);


    return 0;
}