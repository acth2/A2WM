#ifndef MOUSEMOVER_H
#define MOUSEMOVER_H

#include <QThread>
#include <X11/Xlib.h>
#include <QDebug>
#include <unistd.h>

class MouseMover : public QThread {
    Q_OBJECT

public:
    void run() override;
};

#endif // MOUSEMOVER_H
