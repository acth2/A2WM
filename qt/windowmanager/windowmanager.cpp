#include "windowmanager.h"
#include "userinteractright.h"
#include "taskbar.h"
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QKeyEvent>
#include <QSet>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QProcess>
#include <QThread>
#include <QWindow>
#include <QResizeEvent>
#include <QDateTime>
#include <QTransform>
#include <QPushButton>
#include <QStringList>
#include <QRegularExpression>
#include <QStringList>
#include <QList>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

#undef KeyPress
namespace fs = std::filesystem;

// Main window manager constructor
Display *xDisplay;
WindowManager::WindowManager(QWidget *parent)
    : QWidget(parent),
      userInteractRightWidget(nullptr),
      backgroundImagePath("/usr/cydra/backgrounds/current.png") {

    setSupportingWMCheck();

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::Tool | Qt::WindowDoesNotAcceptFocus | Qt::WindowStaysOnBottomHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
        
    QWidget *desktopWidget = new QWidget();
    desktopWidget->setVisible(true);
    layout->addWidget(desktopWidget);
    setLayout(layout);

    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        setGeometry(screenGeometry);
    }

    // Make sure the background is always behind all other windows
    QTimer *backgroundTimer = new QTimer(this);
    connect(backgroundTimer, &QTimer::timeout, this, [this]() {
        Display *display = XOpenDisplay(nullptr);
        if (display) {
            Window window = winId();
            Atom netWMState = XInternAtom(display, "_NET_WM_STATE", False);
            Atom below = XInternAtom(display, "_NET_WM_STATE_BELOW", False);
            XChangeProperty(display, window, netWMState, XA_ATOM, 32, PropModeReplace, (unsigned char *)&below, 1);
            XFlush(display);
            XCloseDisplay(display);
        }
    });
    backgroundTimer->start(500);

    userInteractRightWidget = nullptr;
    showFullScreen();

    // Start KWin
    QProcess *kwinProcess = new QProcess(this);
    QString kwin = "kwin_x11";
    QStringList kwinArgs;
    kwinArgs << "--replace";

    kwinProcess->start(kwin, kwinArgs);
    kwinProcess->start(kwin, kwinArgs);
    kwinProcess->start(kwin, kwinArgs);
        
    connect(kwinProcess, &QProcess::errorOccurred, [](QProcess::ProcessError error) {
        qDebug() << "Error occurred:" << error;
    });

    // Constantly keep the user interactions distracted to avoid suroptimizations:

    QTimer *interactionTimer = new QTimer(this);
    connect(interactionTimer, &QTimer::timeout, this, &WindowManager::simulateUserInteraction);
    interactionTimer->start(2000);
        
    xDisplay = XOpenDisplay(nullptr);
    if (!xDisplay) {
        appendLog("ERR: Failed to open X Display ..");
        return;
    }

    KeyCode keycode = XKeysymToKeycode(xDisplay, XK_Pause);
    if (!keycode) {
        qDebug() << "Failed to get the KEYCODE of XK_Pause";
    }

    XTestFakeKeyEvent(xDisplay, keycode, True, CurrentTime);
    XFlush(xDisplay);
    XCloseDisplay(xDisplay);
}

void WindowManager::simulateUserInteraction() {
    Display *display = XOpenDisplay(nullptr);
    if (!display) {
        appendLog("ERR: Failed to open X Display for fake interaction.");
        return;
    }

    // Simulate a key press and release for the "Control" key
    KeyCode keycode = XKeysymToKeycode(display, XK_Control_L);
    if (keycode) {
        XTestFakeKeyEvent(display, keycode, True, CurrentTime); // Press key
        XTestFakeKeyEvent(display, keycode, False, CurrentTime); // Release key
    } else {
        appendLog("ERR: Failed to get KeyCode for simulated key press.");
    }

    // Simulate a mouse movement to the same position
    Window root = DefaultRootWindow(display);
    int rootX, rootY, winX, winY;
    unsigned int mask;
    Window child, rootReturn;

    if (XQueryPointer(display, root, &rootReturn, &child, &rootX, &rootY, &winX, &winY, &mask)) {
        XTestFakeMotionEvent(display, -1, rootX, rootY, CurrentTime);
    }

    XFlush(display);
    XCloseDisplay(display);
}

void WindowManager::setSupportingWMCheck() {
    // Creating an invisible window that get the usage of helping commands determining the window manager
    xDisplay = XOpenDisplay(nullptr);
    if (!xDisplay) {
        appendLog("ERR: Failed to open X Display ..");
        return;
    }

    Window supportingWindow = XCreateSimpleWindow(xDisplay, DefaultRootWindow(xDisplay), 0, 0, 1, 1, 0, 0, 0);
    if (!supportingWindow) {
        appendLog("ERR: Failed to create supporting window ..");
        return;
    }

    
    Atom netSupportingWMCheck = XInternAtom(xDisplay, "_NET_SUPPORTING_WM_CHECK", False);
    Atom windowId = XInternAtom(xDisplay, "WM_WINDOW", False);
    XChangeProperty(xDisplay, DefaultRootWindow(xDisplay), netSupportingWMCheck, XA_WINDOW, 32, PropModeReplace, (unsigned char *)&supportingWindow, 1);
    Atom netWMState = XInternAtom(xDisplay, "_NET_WM_STATE", False);
    Atom below = XInternAtom(xDisplay, "_NET_WM_STATE_BELOW", False);

    XChangeProperty(xDisplay, supportingWindow, netWMState, XA_ATOM, 32,
                    PropModeReplace, (unsigned char *)&below, 1);

    Atom netWMStateNf = XInternAtom(xDisplay, "_NET_WM_STATE_BELOW", False);
    XChangeProperty(xDisplay, supportingWindow, netWMStateNf, XA_ATOM, 32, PropModeReplace, (unsigned char *)&below, 1);

    XMapWindow(xDisplay, supportingWindow);
    XFlush(xDisplay);
    XCloseDisplay(xDisplay);
}

void WindowManager::appendLog(const QString &message) {
    QFile logFile("/usr/cydra/logs/a2wm.log");
    if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
        qDebug() << "Failed to open log file for appending:" << logFile.errorString();
        return;
    }
    QTextStream out(&logFile);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ") 
        << message << endl;
    logFile.close();
}
bool WindowManager::event(QEvent *qtEvent) {
    if (qtEvent->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(qtEvent);
    } else if (qtEvent->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(qtEvent);
        if (mouseEvent->button() == Qt::RightButton) {
            if (!userInteractRightWidget) {
                userInteractRightWidget = new UserInteractRight(this);
            }
            userInteractRightWidget->move(mouseEvent->globalPos());
            userInteractRightWidget->show();
        }
    }

    // Skip every other action to make the background non-interactive (expect the right click interaction)
    if (qtEvent->type() == QEvent::MouseButtonPress || qtEvent->type() == QEvent::MouseButtonRelease) {
        return true;
    }

    return QWidget::event(qtEvent);
}

void WindowManager::closeEvent(QCloseEvent *event) {
    appendLog("Close attempt ignored");
    event->ignore();
}

void WindowManager::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QPixmap backgroundPixmap(backgroundImagePath);
    if (!backgroundPixmap.isNull()) {
        painter.drawPixmap(0, 0, width(), height(), backgroundPixmap);
    }
}
