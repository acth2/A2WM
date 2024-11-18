#include "mousemover.h"
#include <QTextStream>
#include <QDebug>
#include <X11/Xlib.h>
#include <unistd.h>
#include <QThread>

void MouseMover::run() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        qCritical() << "Unable to open X display";
        return;
    }
    Window root = DefaultRootWindow(display);

    while (true) {
        Window returnedRoot, returnedChild;
        int rootX, rootY, winX, winY;
        unsigned int mask;

        if (XQueryPointer(display, root, &returnedRoot, &returnedChild, 
                          &rootX, &rootY, &winX, &winY, &mask)) {
            XWarpPointer(display, None, root, 0, 0, 0, 0, rootX + 1, rootY);
            XFlush(display);
        }

        usleep(1000);
    }
    XCloseDisplay(display);
}
