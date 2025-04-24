#ifndef SEEER_H_
#define SEEER_H_


#include <X11/X.h>
#include <X11/Xlib.h>


Window findBrowser(Display *display, Window root, int depth);



#endif




// int main(int argc, char *argv[]) {
//     Display *display = XOpenDisplay(NULL);
//     Window root = DefaultRootWindow(display);

//     Window viv = findBrowser(display, root, 0);

//     // printProperties(display, viv);

//     screenshotToFile(display, "screenshot.ppm", viv);


//     return 0;
// }

