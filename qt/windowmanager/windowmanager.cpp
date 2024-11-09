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

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);
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
    showFullScreen();
}

Display *xDisplay;
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
    Atom netWMState = XInternAtom(xDisplay, "_NET_WM_STATE", False);
    Atom below = XInternAtom(xDisplay, "_NET_WM_STATE_BELOW", False);

    XChangeProperty(xDisplay, supportingWindow, netWMState, XA_ATOM, 32,
                    PropModeReplace, (unsigned char *)&below, 1);
    
    XMapWindow(xDisplay, supportingWindow);
    XFlush(xDisplay);
    XCloseDisplay(xDisplay);
}

void WindowManager::toggleConsole() {
    isConsoleVisible = !isConsoleVisible;
    logLabel->setVisible(isConsoleVisible);
    appendLog("Welcome into the DEBUG window (Where my nightmare comes true), Press ESC to exit it");
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
