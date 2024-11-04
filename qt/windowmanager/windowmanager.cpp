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
    connect(windowCheckTimer, &QTimer::timeout, this, &WindowManager::checkForNewWindows);
    windowCheckTimer->start(50);

    showFullScreen();
}

Display *xDisplay;
void WindowManager::listExistingWindows() {
    if (!xDisplay) {
        appendLog("ERR: Failed to open X Display ..");
        return;
    }
    
    Window windowRoot = DefaultRootWindow(xDisplay);
    Window parent, *children = nullptr;
    unsigned int nChildren;

    Window currentWindow;
    int revert_to;
    XGetInputFocus(xDisplay, &currentWindow, &revert_to);
    
    char *currentWindowNameChar = nullptr;
    XFetchName(xDisplay, currentWindow, &currentWindowNameChar);
    QString currentWindowName(currentWindowNameChar);
    XFree(currentWindowNameChar);

    bool currentHasUpperCase = currentWindowName.contains(QRegExp("[A-Z]"));

    if (XQueryTree(xDisplay, windowRoot, &windowRoot, &parent, &children, &nChildren)) {
        for (unsigned int i = 0; i < nChildren; i++) {
            Window child = children[i];
            XWindowAttributes attributes;

            if (XGetWindowAttributes(xDisplay, child, &attributes) == 0 || attributes.map_state != IsViewable) {
                appendLog("INFO: Skipping non-viewable or unmapped window: " + QString::number(child));
                continue;
            }

            char *windowName = nullptr;
            if (XFetchName(xDisplay, child, &windowName) && windowName) {
                QString name(windowName);
                XFree(windowName);

                if (name.isEmpty()) {
                    appendLog("INFO: Skipping No-Name window: " + QString::number(child));
                    continue;
                }

                if (name == "A2WM") {
                    appendLog("INFO: Skipping A2WM windows: " + QString::number(child));
                    continue;
                }

                if (name == "Krusader") {
                    createAndTrackWindow(child, name, attributes.width, attributes.height);
                    continue;
                }

                if (name.toLower() == name && existingWindows.contains(name.toUpper())) {
                    appendLog("INFO: Skipping window with same name (case-sensitive): " + name);
                    continue;
                }

                QSize newSize(attributes.width, attributes.height);
                
                if (existingWindows.contains(name)) {
                    if (existingWindows[name] != newSize) {
                        appendLog("INFO: Skipping window with the same name but different size: " + name);
                        continue;
                    }
                } else {
                    existingWindows.insert(name, newSize);
                    appendLog("Tracking new window: " + name + " with size: " + QString::number(newSize.width()) + "x" + QString::number(newSize.height()));
                }

                existingWindows.insert(name.toUpper(), QSize(0, 0));

                if (trackedWindows.contains(name)) {
                    createAndTrackWindow(child, name, attributes.width, attributes.height);
                    continue;
                }

                appendLog("INFO: Detected new window (WM_NAME): " + name + ", ID: " + QString::number(child));
                
                Atom windowTypeAtom = XInternAtom(xDisplay, "_NET_WM_WINDOW_TYPE", False);
                Atom actualType;
                int format;
                unsigned long nItems, bytesAfter;
                unsigned char *prop = nullptr;

                if (XGetWindowProperty(xDisplay, child, windowTypeAtom, 0, 1024, False,
                    AnyPropertyType, &actualType, &format, &nItems, &bytesAfter, &prop) == Success) {
    
                    bool isMenu = false;
                    if (nItems > 0) {
                        for (unsigned long j = 0; j < nItems; ++j) {
                            Atom type = static_cast<Atom>(prop[j]);
                            if (type == XInternAtom(xDisplay, "_NET_WM_WINDOW_TYPE_MENU", False) ||
                                type == XInternAtom(xDisplay, "_NET_WM_WINDOW_TYPE_DROPDOWN_MENU", False) ||
                                type == XInternAtom(xDisplay, "_NET_WM_WINDOW_TYPE_POPUP_MENU", False)) {
                                isMenu = true;
                                appendLog("INFO: Detected menu window type: " + QString::number(child));
                                break;
                            }
                        }
                    }
                    XFree(prop);

                    if (isMenu) {
                        appendLog("INFO: Skipping menu window: " + QString::number(child));
                        continue;
                    }
                }

                createAndTrackWindow(child, name, attributes.width, attributes.height);
            }
        }
        XFree(children);
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
    xDisplay = XOpenDisplay(nullptr);
    if (xDisplay) {
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
        XCloseDisplay(xDisplay);
    } else {
        appendLog("ERR: Failed to open X Display ..");
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
    XEvent event;
    if (xDisplay) {
        while (XPending(xDisplay)) {
            XNextEvent(xDisplay, &event);

            if (event.type == ConfigureNotify) {
                XConfigureEvent xce = event.xconfigure;

                if (trackedWindows.contains(xce.window)) {
                    QWindow *window = trackedWindows.value(xce.window);
                    QRect windowGeometry = window->geometry();

                    appendLog(QString("INFO: Window resized/moved: (%1, %2), Size: (%3x%4)")
                        .arg(xce.x).arg(xce.y)
                        .arg(xce.width).arg(xce.height));

                    updateTaskbarPosition(window);
                }
            }

            if (event.type == PropertyNotify) {
                XPropertyEvent *propEvent = (XPropertyEvent *)&event;

                if (propEvent->atom == XInternAtom(xDisplay, "_NET_WM_STATE", False)) {
                    if (trackedWindows.contains(propEvent->window)) {
                        QWindow *window = trackedWindows.value(propEvent->window);
                        QWidget *container = trackedContainers.value(propEvent->window);

                        Atom fullscreenAtom = XInternAtom(xDisplay, "_NET_WM_STATE_FULLSCREEN", False);
                        Atom netWmState = XInternAtom(xDisplay, "_NET_WM_STATE", False);

                        Atom actualType;
                        int actualFormat;
                        unsigned long nItems, bytesAfter;
                        unsigned char *prop = nullptr;

                        int status = XGetWindowProperty(xDisplay, propEvent->window, netWmState, 0, (~0L), False, XA_ATOM, 
                                                        &actualType, &actualFormat, &nItems, &bytesAfter, &prop);

                        if (status == Success && prop) {
                            Atom *atoms = (Atom *)prop;
                            bool isFullscreen = false;

                            for (unsigned long i = 0; i < nItems; ++i) {
                                if (atoms[i] == fullscreenAtom) {
                                    isFullscreen = true;
                                    break;
                                }
                            }

                            XFree(prop);

                            if (isFullscreen) {
                                appendLog("INFO: Window is fullscreen, adjusting container size.");
                                QScreen *screen = QApplication::primaryScreen();
                                QRect screenGeometry = screen->geometry();
                                container->setGeometry(screenGeometry);
                            } else {
                                appendLog("INFO: Window exited fullscreen, restoring container size.");
                                container->setGeometry(window->geometry());
                            }
                        }
                    }
                }

                if (propEvent->atom == XInternAtom(xDisplay, "_NET_WM_WINDOW_OPACITY", False)) {
                    if (trackedWindows.contains(propEvent->window)) {
                        QWindow *window = trackedWindows.value(propEvent->window);
                        QWidget *container = trackedContainers.value(propEvent->window);

                        Atom opacityAtom = XInternAtom(xDisplay, "_NET_WM_WINDOW_OPACITY", False);
                        unsigned long opacityValue = 0xffffffff;

                        Atom actualType;
                        int actualFormat;
                        unsigned long nItems, bytesAfter;
                        unsigned char *prop = nullptr;

                        int status = XGetWindowProperty(xDisplay, propEvent->window, opacityAtom, 0, (~0L), False, XA_CARDINAL,
                                                        &actualType, &actualFormat, &nItems, &bytesAfter, &prop);

                        if (status == Success && prop) {
                            opacityValue = *(unsigned long *)prop;
                            XFree(prop);

                            qreal opacity = static_cast<qreal>(opacityValue) / 0xffffffff;

                            appendLog(QString("INFO: Window opacity changed: %1").arg(opacity));

                            container->setWindowOpacity(opacity);
                        }
                    }
                }
            }
        }
    } else {
        appendLog("ERR: Failed to open X Display ..");
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
    QList<WId> windowsToRemove;
    for (auto xorgWindowId : trackedWindows.keys()) {
        XWindowAttributes attributes;
        int status = XGetWindowAttributes(xDisplay, xorgWindowId, &attributes);

        if (status == 0 || attributes.map_state == IsUnmapped) {
            windowsToRemove.append(xorgWindowId);
        }
    }

    for (auto xorgWindowId : windowsToRemove) {
        QWindow *window = trackedWindows.value(xorgWindowId);
        trackedWindows.remove(xorgWindowId);

        if (windowTopBars.contains(xorgWindowId)) {
            TopBar *topBar = windowTopBars.value(xorgWindowId);
            topBar->hide();
            topBar->deleteLater();
            windowTopBars.remove(xorgWindowId);
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
