#include "windowmanager.h"
#include "win/topbar.h"
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

#undef KeyPress
namespace fs = std::filesystem;
Display *xDisplay = nullptr;

WindowManager::WindowManager(QWidget *parent)
    : QWidget(parent),
      isConsoleVisible(false),
      userInteractRightWidget(nullptr),
      resizeMode(false),
      backgroundImagePath("/usr/cydra/backgrounds/current.png") {

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setSupportingWMCheck();

    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        setGeometry(screenGeometry);
    }

    xDisplay = XOpenDisplay(nullptr);
    if (!xDisplay) {
        appendLog("ERR: Failed to open X Display ..");
        return;
    }
    Window root = DefaultRootWindow(xDisplay);
    XSelectInput(xDisplay, root, SubstructureNotifyMask | SubstructureRedirectMask);

    logLabel = new QLabel(this);
    logLabel->setStyleSheet("QLabel { color : white; background-color : rgba(0, 0, 0, 150); }");
    logLabel->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    logLabel->setVisible(false);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(logLabel);
    layout->setContentsMargins(10, 10, 10, 10);
    QWidget *desktopWidget = new QWidget();
    desktopWidget->setVisible(true);
    layout->addWidget(desktopWidget);
    setLayout(layout);

    konamiCodeHandler = new KonamiCodeHandler(this);
    connect(konamiCodeHandler, &KonamiCodeHandler::konamiCodeEntered, this, &WindowManager::toggleConsole);

    userInteractRightWidget = nullptr;
        
    windowCheckTimer = new QTimer(this);
    connect(windowCheckTimer, &QTimer::timeout, this, &WindowManager::processX11Events);
    windowCheckTimer->start(50);

    showFullScreen();
}

QSet<QString> whitelist;
void WindowManager::loadWhitelist() {
    QFile file("/usr/cydra/settings/whitelist");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        appendLog("ERR: Failed to open whitelist file.");
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            whitelist.insert(line);
        }
    }
    file.close();
}

QSet<WId> trackedWindows;
void WindowManager::listExistingWindows() {
    while (XPending(xDisplay)) {
        XEvent event;
        XNextEvent(xDisplay, &event);

        if (event.type == CreateNotify) {
            XCreateWindowEvent* createEvent = (XCreateWindowEvent*)&event;

            Window newWindow = createEvent->window;
            XWindowAttributes attributes;
            XGetWindowAttributes(xDisplay, newWindow, &attributes);

            XTextProperty windowNameProperty;
            QString windowName;
            if (XGetWMName(xDisplay, newWindow, &windowNameProperty) && windowNameProperty.value) {
                windowName = QString::fromUtf8(reinterpret_cast<const char*>(windowNameProperty.value));
                XFree(windowNameProperty.value);
            }

            int width = attributes.width;
            int height = attributes.height;

            bool trackingEligible = !whitelist.contains(windowName) && windowName != "A2WM";

            if (trackingEligible) {
                createAndTrackWindow(newWindow, windowName, width, height);
            }
        }
        else if (event.type == MapRequest) {
            XMapRequestEvent* mapRequestEvent = (XMapRequestEvent*)&event;
            XMapWindow(xDisplay, mapRequestEvent->window);
        }
    }
}

void WindowManager::setSupportingWMCheck() {
    xDisplay = XOpenDisplay(nullptr);
    if (!xDisplay) {
        appendLog("ERR: Failed to open X Display ..");
        return;
    }

    Window supportingWindow = XCreateSimpleWindow(xDisplay, DefaultRootWindow(xDisplay), 0, 0, 1, 1, 0, 0, 0);
    
    Atom netSupportingWMCheck = XInternAtom(xDisplay, "_NET_SUPPORTING_WM_CHECK", False);
    Atom windowId = XInternAtom(xDisplay, "WM_WINDOW", False);
    XChangeProperty(xDisplay, DefaultRootWindow(xDisplay), netSupportingWMCheck, XA_WINDOW, 32, PropModeReplace, (unsigned char *)&supportingWindow, 1);
    
    XMapWindow(xDisplay, supportingWindow);
    XFlush(xDisplay);
    XCloseDisplay(xDisplay);
}

void WindowManager::checkForNewWindows() {
    if (!xDisplay) {
        appendLog("ERR: X Display is not open.");
        return;
    }

    loadWhitelist();
    listExistingWindows();
    processX11Events();
    cleanUpClosedWindows();

    Window activeWindow;
    int revert;
    XGetInputFocus(xDisplay, &activeWindow, &revert);
    if (!trackedWindows.contains(activeWindow)) {
        appendLog("INFO: Focusing back to Qt window");
        this->activateWindow();
    }
}


void WindowManager::trackWindowEvents(Window xorgWindowId) {
    xDisplay = XOpenDisplay(nullptr);
    if (xDisplay) {
        XSelectInput(xDisplay, xorgWindowId, StructureNotifyMask);
    } else {
        appendLog("ERR: Failed to open X Display ..");
    }
}

void WindowManager::processX11Events() {
    if (!xDisplay) {
        appendLog("ERR: X Display is not open.");
        return;
    }

    if (XPending(xDisplay) > 0) {
        XEvent event;
        XNextEvent(xDisplay, &event);

        switch (event.type) {
            case CreateNotify: {
                XCreateWindowEvent* createEvent = (XCreateWindowEvent*)&event;
                Window newWindow = createEvent->window;
                
                XWindowAttributes attributes;
                XGetWindowAttributes(xDisplay, newWindow, &attributes);

                XTextProperty windowNameProperty;
                QString windowName;
                if (XGetWMName(xDisplay, newWindow, &windowNameProperty) && windowNameProperty.value) {
                    windowName = QString::fromUtf8(reinterpret_cast<const char*>(windowNameProperty.value));
                    XFree(windowNameProperty.value);
                }

                int width = attributes.width;
                int height = attributes.height;
                bool trackingEligible = !whitelist.contains(windowName) && windowName != "A2WM";

                if (trackingEligible) {
                    createAndTrackWindow(newWindow, windowName, width, height);
                }
                break;
            }
            case DestroyNotify: {
                Window closedWindow = event.xdestroywindow.window;
                auto it = std::find_if(trackedWindows.begin(), trackedWindows.end(),
                                       [closedWindow](QWindow* w) { return w->winId() == closedWindow; });
                if (it != trackedWindows.end()) {
                    trackedWindows.erase(it);
                    appendLog("INFO: Cleaned up closed window.");
                }
                break;
            }
            case MapRequest: {
                XMapRequestEvent* mapRequestEvent = (XMapRequestEvent*)&event;
                XMapWindow(xDisplay, mapRequestEvent->window);
                break;
            }
        }
    }
}

void WindowManager::toggleConsole() {
    isConsoleVisible = !isConsoleVisible;
    logLabel->setVisible(isConsoleVisible);
    appendLog("Welcome into the DEBUG window (Where my nightmare comes true), Press ESC to exit it");
}

void WindowManager::createAndTrackWindow(WId xorgWindowId, QString windowName, int width, int height) {
    appendLog(QString("INFO: Creating and tracking window: %1").arg(xorgWindowId));

    QWindow *x11Window = QWindow::fromWinId(xorgWindowId);
    if (!x11Window) {
        appendLog("ERR: Failed to create QWindow from X11 ID.");
        return;
    }

    trackedWindows.insert(xorgWindowId, x11Window);

    QWidget *containerWidget = new QWidget(this);
    if (!containerWidget) {
        appendLog("ERR: Failed to create container widget.");
        return;
    }

    int topbarHeight = 30;

    containerWidget->setGeometry(x11Window->geometry().x(), x11Window->geometry().y(), width, height + topbarHeight);

    QWidget *windowWidget = QWidget::createWindowContainer(x11Window, containerWidget);
    if (!windowWidget) {
        appendLog("ERR: Failed to create window container.");
        return;
    }

    QVBoxLayout *layout = new QVBoxLayout(containerWidget);
    layout->addWidget(windowWidget);

    TopBar *topBar = new TopBar(x11Window, this);
    if (!topBar) {
        appendLog("ERR: Failed to create TopBar.");
        return;
    }

    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int centerX = (screenGeometry.width() - width) / 2;
        int centerY = (screenGeometry.height() - height) / 2;

        containerWidget->move(centerX, centerY);
        appendLog("INFO: Centered window ID " + QString::number(xorgWindowId) + " at " 
                  + QString::number(centerX) + ", " + QString::number(centerY));
    }

    topBar->setGeometry(containerWidget->geometry().x(), containerWidget->geometry().y() - topbarHeight,
                        containerWidget->geometry().width(), topbarHeight);

    topBar->setTitle(windowName);
    topBar->show();
    containerWidget->show();

    appendLog(QString("INFO: Successfully created container and TopBar for window: %1").arg(xorgWindowId));

    windowTopBars.insert(xorgWindowId, topBar);
    trackedContainers.insert(xorgWindowId, containerWidget);

    topBar->updatePosition();
}


void WindowManager::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && resizeMode) {
        resizeMode = false;
    }
    return QWidget::mouseReleaseEvent(event);
}

bool WindowManager::eventFilter(QObject *object, QEvent *event) {
    return QWidget::eventFilter(object, event);
}

void WindowManager::mouseMoveEvent(QMouseEvent *event) {
    QWidget::mouseMoveEvent(event);
}

void WindowManager::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}

void WindowManager::closeWindow(WId windowId) {
    if (trackedWindows.contains(windowId)) {
        QWindow* window = trackedWindows.value(windowId);
        if (window) {
            window->hide();
            trackedWindows.remove(windowId);
            appendLog("INFO: Window killed");
        }
    }
}

void WindowManager::updateTaskbarPosition(QWindow *window) {
    if (windowTopBars.contains(window->winId())) {
        TopBar *topBar = windowTopBars.value(window->winId());
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        
        int windowWidth = window->width();
        int windowHeight = window->height();
        int topbarHeight = 30;

        int centeredX = (screenGeometry.width() - windowWidth) / 2;
        int centeredY = (screenGeometry.height() - windowHeight) / 2;

        if (windowWidth <= 0 || windowHeight <= 0) {
            windowWidth = 800;
            windowHeight = 600;
            centeredX = (screenGeometry.width() - windowWidth) / 2;
            centeredY = (screenGeometry.height() - windowHeight) / 2;
        }

        window->setGeometry(centeredX, centeredY, windowWidth, windowHeight);

        topBar->setGeometry(centeredX, centeredY - topbarHeight, windowWidth, topbarHeight);
        topBar->show();
    }
}

void WindowManager::appendLog(const QString &message) {
    QFile logFile("/usr/cydra/logs/cwm.log");
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ") 
            << message << endl;
        logFile.close();
    }

    if (!loggedMessages.contains(message)) {
        loggedMessages.insert(message);
        QString currentText = logLabel->text();
        logLabel->setText(currentText + "\n" + message);
    }
}

bool WindowManager::event(QEvent *qtEvent) {
    if (qtEvent->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(qtEvent);
        konamiCodeHandler->handleKeyPress(keyEvent);
    } else if (qtEvent->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(qtEvent);
        if (mouseEvent->button() == Qt::RightButton) {
            if (!userInteractRightWidget) {
                userInteractRightWidget = new UserInteractRight(this);
            }
            userInteractRightWidget->move(mouseEvent->globalPos());
            userInteractRightWidget->show();
        } else if (mouseEvent->button() == Qt::LeftButton) {
            if (!this->geometry().contains(mouseEvent->pos())) {
                appendLog("INFO: Clicking outside Qt window, refocusing");
                this->activateWindow();
            }
        }
    }

    return QWidget::event(qtEvent);
}

void WindowManager::cleanUpClosedWindows() {
    QSet<WId> windowsToRemove;
    for (QWindow* window : trackedWindows) {
        WId windowId = window->winId();

        XWindowAttributes attributes;
        if (XGetWindowAttributes(xDisplay, windowId, &attributes) == 0 || attributes.map_state == IsUnmapped) {
            windowsToRemove.insert(windowId);
        }
    }

    for (WId windowId : windowsToRemove) {
        auto it = std::find_if(trackedWindows.begin(), trackedWindows.end(),
                               [windowId](QWindow* w) { return w->winId() == windowId; });
        if (it != trackedWindows.end()) {
            trackedWindows.erase(it);
            appendLog("INFO: Cleaned up closed window.");
        }
    }
}


void WindowManager::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape && logLabel->isVisible()) {
        toggleConsole();
    } else {
        QWidget::keyPressEvent(event);
    }
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
