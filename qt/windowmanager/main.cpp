#include "windowmanager.h"
#include "taskbar.h"
#include <QApplication>
#include <QScreen>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QtMessageHandler>
#include <QMessageBox>
#include <QGuiApplication>
#include <QX11Info>
#include <QThread>
#include <QDebug>
#include <cstdlib>
#include <X11/Xlib.h>

QFile logFile;

// Custom logging function
void customLogOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QTextStream out(&logFile);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
        << msg << Qt::endl;
}

// Worker class for mouse movement
class MouseMover : public QThread {
    Q_OBJECT

public:
    void run() override {
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

            usleep(1000); // Sleep for 1 millisecond
        }
        XCloseDisplay(display);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    logFile.setFileName("/usr/cydra/logs/a2wm.log");
    logFile.open(QIODevice::Append | QIODevice::Text);

    qInstallMessageHandler(customLogOutput);

    // Check if the user use Wayland
    QString platform = QGuiApplication::platformName();
    if (!QX11Info::isPlatformX11()) {
        QMessageBox::critical(nullptr, "Error", "This window manager only works on X11/Xorg.\nPlease restart A2WM with X11\n\n[PRESS ENTER TO CONTINUE]");
        return -1;
    }
    
    WindowManager manager;
    TaskBar taskBar;

    // Create the window manager instance
    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        manager.setGeometry(screenGeometry);
    }
    
    manager.setWindowTitle("A2WM");
    manager.showFullScreen();

    // Create the taskbar
    taskBar.show();

    // Start the mouse mover in a separated thread
    MouseMover mouseMover;
    mouseMover.start();
    // /\ Help the window manager keep the actives windows enabled and not making them lag
    
    return app.exec();
}
