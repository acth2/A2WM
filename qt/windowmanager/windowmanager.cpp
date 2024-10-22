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
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_ewmh.h>

#undef KeyPress
namespace fs = std::filesystem;

xcb_connection_t *connection;
xcb_screen_t *screen;

WindowManager::WindowManager(QWidget *parent)
    : QWidget(parent),
      isConsoleVisible(false),
      userInteractRightWidget(nullptr),
      resizeMode(false),
      backgroundImagePath("/usr/cydra/backgrounds/current.png") {
    
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setSupportingWMCheck();

    if (QScreen *screen = QApplication::primaryScreen())
        setGeometry(screen->geometry());

    logLabel = new QLabel(this);
    logLabel->setStyleSheet("QLabel { color : white; background-color : rgba(0, 0, 0, 150); }");
    logLabel->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    logLabel->setVisible(false);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(logLabel);
    layout->setContentsMargins(10, 10, 10, 10);
    setLayout(layout);

    konamiCodeHandler = new KonamiCodeHandler(this);
    connect(konamiCodeHandler, &KonamiCodeHandler::konamiCodeEntered, this, &WindowManager::toggleConsole);

    userInteractRightWidget = nullptr;

    windowCheckTimer = new QTimer(this);
    connect(windowCheckTimer, &QTimer::timeout, this, &WindowManager::checkForNewWindows);
    windowCheckTimer->start(50);

    auto *desktopUpdateTimer = new QTimer(this);
    connect(desktopUpdateTimer, &QTimer::timeout, this, &WindowManager::updateDesktopIcons);
    desktopUpdateTimer->start(1000);

    showFullScreen();
}

void WindowManager::updateDesktopIcons() {
    appendLog("Desktop Icons timer updated!");
}

void WindowManager::activateWindow() {
    raise();
    setFocus();
}

void WindowManager::initXCBConnection() {
    connection = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(connection)) {
        appendLog("ERR: Failed to connect to X server via XCB.");
        return;
    }

    const auto *setup = xcb_get_setup(connection);
    auto iter = xcb_setup_roots_iterator(setup);
    xcb_screen_t *screen;
    screen = iter.data;

    initXCBAtoms();
}

void WindowManager::initXCBAtoms() {
    netWmStateFullscreen = getAtom("_NET_WM_STATE_FULLSCREEN");
    netWmStateMaximizedVert = getAtom("_NET_WM_STATE_MAXIMIZED_VERT");
    netWmStateMaximizedHorz = getAtom("_NET_WM_STATE_MAXIMIZED_HORZ");
}

xcb_atom_t WindowManager::getAtom(const char *atomName) {
    auto atomCookie = xcb_intern_atom(connection, 0, strlen(atomName), atomName);
    auto *atomReply = xcb_intern_atom_reply(connection, atomCookie, nullptr);
    if (!atomReply) return XCB_ATOM_NONE;

    xcb_atom_t atom = atomReply->atom;
    free(atomReply);
    return atom;
}

void WindowManager::listExistingWindows() {
    if (!connection) {
        appendLog("ERR: XCB connection not initialized.");
        return;
    }

    xcb_screen_t *screen = iter.data;
    auto cookie = xcb_ewmh_get_client_list_unchecked(&ewmh, screen->root);
    xcb_ewmh_get_windows_reply_t reply;
    if (!xcb_ewmh_get_client_list_reply(&ewmh, cookie, &reply, nullptr)) {
        appendLog("ERR: Failed to get _NET_CLIENT_LIST.");
        return;
    }

    for (unsigned int i = 0; i < reply.windows_len; ++i) {
        xcb_window_t window = reply.windows[i];

        auto nameCookie = xcb_ewmh_get_wm_name_unchecked(&ewmh, window);
        xcb_ewmh_get_utf8_strings_reply_t nameReply;
        if (!xcb_ewmh_get_wm_name_reply(&ewmh, nameCookie, &nameReply, nullptr)) continue;

        QString windowName = QString::fromUtf8(nameReply.strings, nameReply.strings_len);

        if (windowName == "A2WM") {
            appendLog("INFO: A2WM window detected, skipping ID: " + QString::number(window));
            continue;
        }

        if (windowName == "QTerminal" || windowName == "Shell No. 1") {
            appendLog("INFO: Detected QTerminal window: " + QString::number(window));
            createAndTrackWindow(window, "QTerminal");
            continue;
        }

        if (windowName == "A2WMEdit" || windowName == "Fadyedit") {
            appendLog("INFO: Detected A2WMEdit / FadyEdit window: " + QString::number(window));
            createAndTrackWindow(window, "A2WMEdit");
            continue;
        }

        appendLog("INFO: Detected graphical XCB window: " + QString::number(window));
        if (!trackedWindows.contains(window)) 
            createAndTrackWindow(window, windowName);

        xcb_ewmh_get_utf8_strings_reply_wipe(&nameReply);
    }

    xcb_ewmh_get_windows_reply_wipe(&reply);
}

Display *xDisplay;
void WindowManager::setSupportingWMCheck() {
    connection = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(connection)) {
        appendLog("ERR: Failed to connect to X server via XCB.");
        return;
    }

    xDisplay = XOpenDisplay(nullptr);
    if (!xDisplay) {
        appendLog("ERR: Failed to open X Display.");
        return;
    }

    Window supportingWindow = XCreateSimpleWindow(xDisplay, DefaultRootWindow(xDisplay), 0, 0, 1, 1, 0, 0, 0);

    Atom netSupportingWMCheck = XInternAtom(xDisplay, "_NET_SUPPORTING_WM_CHECK", False);
    Atom windowId = XInternAtom(xDisplay, "WM_WINDOW", False);

    xcb_screen_t *screen = iter.data;
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, screen->root, netSupportingWMCheck, XCB_ATOM_WINDOW, 32, 1, &supportingWindow);
    
    XMapWindow(xDisplay, supportingWindow);
    XFlush(xDisplay);
    xcb_disconnect(connection);
}

void WindowManager::checkForNewWindows() {
    connection = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(connection)) {
        appendLog("ERR: Failed to connect to X server via XCB.");
        return;
    }

    listExistingWindows();
    processX11Events();
    cleanUpClosedWindows();

    xcb_window_t activeWindow = 0;
    auto focusCookie = xcb_get_input_focus(connection);
    auto *focusReply = xcb_get_input_focus_reply(connection, focusCookie, nullptr);
    if (focusReply) {
        xcb_window_t activeWindow = focusReply->focus;
        appendLog(QString("INFO: Active window ID: %1").arg(activeWindow));
        free(focusReply);
    } else {
        appendLog("ERR: Failed to get input focus via XCB.");
    }

    if (!trackedWindows.contains(activeWindow)) {
        appendLog("INFO: Focusing back to Qt window");
        activateWindow();
    }

    xcb_disconnect(connection);
}

void WindowManager::trackWindowEvents(Window xorgWindowId) {
    connection = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(connection)) {
        appendLog("ERR: Failed to connect to X server via XCB.");
        return;
    }

    uint32_t values[] = {XCB_EVENT_MASK_STRUCTURE_NOTIFY};
    xcb_change_window_attributes(connection, xorgWindowId, XCB_CW_EVENT_MASK, values);
}

void WindowManager::processX11Events() {
    xcb_generic_event_t *event;
    while ((event = xcb_poll_for_event(connection))) {
        uint8_t eventType = event->response_type & ~0x80;

        if (eventType == XCB_CONFIGURE_NOTIFY) {
            auto *configureEvent = reinterpret_cast<xcb_configure_notify_event_t *>(event);
            appendLog(QString("INFO: Window resized: (%1x%2)").arg(configureEvent->width).arg(configureEvent->height));
        }

        free(event);
    }
}

void WindowManager::toggleConsole() {
    isConsoleVisible = !isConsoleVisible;
    logLabel->setVisible(isConsoleVisible);
    appendLog("Welcome into the DEBUG window (Where my nightmare comes true), Press ESC to exit it");
}

void WindowManager::createAndTrackWindow(WId xorgWindowId, QString windowName) {
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

    QRect geometry = x11Window->geometry();
    int topbarHeight = 30;

    if (geometry.isValid()) {
        containerWidget->setGeometry(geometry.x(), geometry.y(), geometry.width(), geometry.height() + topbarHeight);
    } else {
        containerWidget->setGeometry(50, 80, 400, 400 + topbarHeight);
    }

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
        xcb_get_geometry_cookie_t geomCookie = xcb_get_geometry(connection, xorgWindowId);
        xcb_get_geometry_reply_t *geomReply = xcb_get_geometry_reply(connection, geomCookie, nullptr);
        if (geomReply) {
            appendLog(QString("INFO: Window width: %1, height: %2").arg(geomReply->width).arg(geomReply->height));
            free(geomReply);
        } else {
            appendLog("ERR: Failed to get window geometry via XCB.");
        }

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
