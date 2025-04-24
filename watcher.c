#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xrender.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define OUTFILE "vivaldi.rgb"

Window find_vivaldi_window(Display *dpy, Window root) {
    Window parent;
    Window *children;
    unsigned int nchildren;
    char *window_name = NULL;
    Window result = 0;

    if (XQueryTree(dpy, root, &root, &parent, &children, &nchildren)) {
        for (unsigned int i = 0; i < nchildren; ++i) {
            result = find_vivaldi_window(dpy, children[i]); // recurse first
            if (result) break;

            if (XFetchName(dpy, children[i], &window_name)) {
                if (window_name && strstr(window_name, "vivaldi")) {
                    result = children[i];
                    XFree(window_name);
                    break;
                }
                XFree(window_name);
            }
        }
        if (children) XFree(children);
    }
    return result;
}

int main() {
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Failed to open X display\n");
        return 1;
    }

    int event_base, error_base;
    if (!XCompositeQueryExtension(dpy, &event_base, &error_base)) {
        fprintf(stderr, "XComposite not available\n");
        return 1;
    }

    Window root = DefaultRootWindow(dpy);
    Window win = find_vivaldi_window(dpy, root);
    if (!win) {
        fprintf(stderr, "Could not find Vivaldi window\n");
        return 1;
    }

    XWindowAttributes attr;
    XGetWindowAttributes(dpy, win, &attr);

    if (attr.map_state != IsViewable) {
        fprintf(stderr, "Window not viewable (maybe minimized?)\n");
        return 1;
    }

    // Redirect manually — safer
    XCompositeRedirectSubwindows(dpy, root, CompositeRedirectManual);
    XSync(dpy, False);

    Pixmap pixmap = XCompositeNameWindowPixmap(dpy, win);
    if (!pixmap) {
        fprintf(stderr, "Failed to get window pixmap\n");
        return 1;
    }

    XImage *image = XGetImage(dpy, pixmap, 0, 0, attr.width, attr.height, AllPlanes, ZPixmap);
    if (!image) {
        fprintf(stderr, "Failed to capture image\n");
        return 1;
    }

    FILE *f = fopen(OUTFILE, "wb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    // Save as raw RGB
    for (int y = 0; y < attr.height; ++y) {
        for (int x = 0; x < attr.width; ++x) {
            unsigned long pixel = XGetPixel(image, x, y);
            unsigned char r = (pixel & image->red_mask) >> 16;
            unsigned char g = (pixel & image->green_mask) >> 8;
            unsigned char b = (pixel & image->blue_mask);
            fputc(r, f);
            fputc(g, f);
            fputc(b, f);
        }
    }

    fclose(f);
    XDestroyImage(image);
    XCloseDisplay(dpy);

    printf("Screenshot saved to %s\n", OUTFILE);
    return 0;
}
