#include "topbar.h"
#include "MinimizedPosArray.h"
#include "../windowmanager.h"
#include <QApplication>
#include <QMouseEvent>
#include <QPushButton>
#include <QProcess>
#include <QPainter>
#include <QObject>
#include <QString>
#include <QGuiApplication>
#include <QScreen>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QFile>
#include <QFocusEvent>
#include <QWidget>

TopBar::TopBar(QWindow *parentWindow, WindowManager *manager, QWidget *parent)
    : QWidget(parent), trackedWindow(parentWindow), isDragging(false) {

    if (QFile::exists("/usr/cydra/settings/darkmode")) {
        isDarkMode = true;
    } else {
        isDarkMode = false;
    }
        
    trackedWindow->installEventFilter(this);
    isMaximized = false;

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::NoFocus);

    titleLabel = new QLabel(this);
    titleLabel->setStyleSheet("QLabel { color: white; }");

    popup = new QLabel(this);
    popup->setFixedSize(500, 500);
        
    closeButton = new QPushButton("✕", this);
    closeButton->setFixedSize(30, 30);
    if (!isDarkMode){
        closeButton->setStyleSheet(
            "QPushButton {"
            "   border-radius: 15px;"
            "   background-color: black;"
            "   color: white;"
            "   border: none;"
            "   font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "   background-color: red;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #8B0000;"
            "}"
        );
    } else {
        closeButton->setStyleSheet(
            "QPushButton {"
            "   border-radius: 15px;"
            "   background-color: white;"
            "   color: black;"
            "   border: none;"
            "   font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "   background-color: red;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #8B0000;"
            "}"
        );
    }

    maximizeButton = new QPushButton(this);
    maximizeButton->setText("❐");
    maximizeButton->setFixedSize(30, 30);
    if (!isDarkMode){
        maximizeButton->setStyleSheet(
            "QPushButton {"
            "   background-color: black;"
            "   border-radius: 15px;"
            "   font-size: 18px;"
            "}"
            "QPushButton:hover {"
            "   background-color: gray;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #404040;"
            "}"
        );
    } else {
        maximizeButton->setStyleSheet(
            "QPushButton {"
            "   background-color: white;"
            "   border-radius: 15px;"
            "   font-size: 18px;"
            "}"
            "QPushButton:hover {"
            "   background-color: gray;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #404040;"
            "}"
        );
    }

    minusButton = new QPushButton(this);
    minusButton->setText("−");
    minusButton->setFixedSize(30, 30);
    if (!isDarkMode){
        minusButton->setStyleSheet(
            "QPushButton {"
            "   background-color: black;"
            "   border-radius: 15px;"
            "   font-size: 18px;"
            "}"
            "QPushButton:hover {"
            "   background-color: gray;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #404040;"
            "}"
        );
    } else {
        minusButton->setStyleSheet(
            "QPushButton {"
            "   background-color: white;"
            "   border-radius: 15px;"
            "   font-size: 18px;"
            "}"
            "QPushButton:hover {"
            "   background-color: gray;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #404040;"
            "}"
        );
    }

    resizeButton = new QPushButton(this);
    resizeButton->setText("⌞⌝");
    resizeButton->setFixedSize(30, 30);
    if (!isDarkMode){
        resizeButton->setStyleSheet(
            "QPushButton {"
            "   background-color: black;"
            "   border-radius: 15px;"
            "   font-size: 18px;"
            "}"
            "QPushButton:hover {"
            "   background-color: gray;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #404040;"
            "}"
        );
    } else {
        resizeButton->setStyleSheet(
            "QPushButton {"
            "   background-color: white;"
            "   border-radius: 15px;"
            "   font-size: 18px;"
            "}"
            "QPushButton:hover {"
            "   background-color: gray;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #404040;"
            "}"
        );
    }

        
    connect(closeButton, &QPushButton::clicked, this, &TopBar::closeTrackedWindow);
    connect(maximizeButton, &QPushButton::clicked, this, &TopBar::toggleMaximizeRestore);
    connect(minusButton, &QPushButton::clicked, this, &TopBar::minimizeWindow);
    connect(resizeButton, &QPushButton::clicked, this, &TopBar::startResizing);
        
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(titleLabel);
    layout->addStretch();
    layout->addWidget(resizeButton);
    layout->addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));
    layout->addWidget(minusButton);
    layout->addWidget(maximizeButton);
    layout->addWidget(closeButton);
    layout->setContentsMargins(10, 5, 10, 2);
    
    setLayout(layout);
    updatePosition();
}

void TopBar::minimizeWindow() {
    if (trackedWindow && !isMinimized) {
        originalGeometry = trackedWindow->geometry();
        
        originalTopBarWidth = this->width();

        trackedWindow->setGeometry(trackedWindow->x(), trackedWindow->y(), 0, 0);

        maximizeButton->hide();
        closeButton->hide();
        minusButton->hide();
        resizeButton->hide();

        int minimizedX = MinimizedPosArray::getInstance().getSmallestAvailable();
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();

        origTitle = titleLabel->text();

        if (origTitle.size() > 9) {
            QString newTitle = origTitle.left(9) + "-";
            titleLabel->setText(newTitle);
        }

        this->resize(25, this->height());

        this->setGeometry(minimizedX + 75, screenGeometry.height() - 38, 25, this->height());

        MinimizedPosArray::getInstance().markPositionAsTaken(minimizedX);
        
        isMinimized = true;
    }
}

void TopBar::focusInEvent(QFocusEvent *event) {
    QWidget::focusInEvent(event);
    this->show();
}

QWindow* TopBar::getTrackedWindow() const {
    return trackedWindow;
}

QLabel* TopBar::getPopup() const {
    return popup;
}

bool TopBar::eventFilter(QObject *obj, QEvent *event) {
    if (obj == trackedWindow) {
        if (event->type() == QEvent::WindowActivate) {
            if (!isMinimized) {
                this->show();
            }
        }
    }

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        if (getTrackedWindow()) {
            getTrackedWindow()->requestActivate();
            getTrackedWindow()->raise();
        }

        if (getPopup()->isVisible() && !getPopup()->geometry().contains(mouseEvent->globalPos())) {
            closePopup();
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}

void TopBar::updatePosition() {
    if (trackedWindow) {
        if (isMinimized) {
            return;
        }
        QRect windowGeometry = trackedWindow->geometry();
        int topbarHeight = 36;
        setGeometry(windowGeometry.x(), windowGeometry.y() - topbarHeight, windowGeometry.width(), topbarHeight);
        show();
    }
}

void TopBar::setTitle(const QString &title) {
    titleLabel->setText(title);
}

void TopBar::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if (!isDarkMode) {
        painter.setBrush(QColor(255, 255, 255, 150));    
    } else {
        painter.setBrush(QColor(51, 51, 51, 150));
    }
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());
}

void TopBar::startResizing() {
    if (trackedWindow) {
        QPoint originalCursorPos = QCursor::pos();
        maximizeButton->setVisible(false);
        
        QRect windowGeometry = trackedWindow->geometry();
        QPoint bottomRightCorner = windowGeometry.bottomRight();
        QCursor::setPos(bottomRightCorner);

        QTimer::singleShot(1, this, [this, originalCursorPos]() {
            isResizing = true;
            setCursor(Qt::SizeAllCursor);
            resizeStartPos = QCursor::pos();
            windowStartSize = trackedWindow->size();

            QCursor::setPos(originalCursorPos);
        });
    }
}

void TopBar::stopResizing() {
    isResizing = false;
    maximizeButton->setVisible(true);
}

void TopBar::mousePressEvent(QMouseEvent *event) {
    if (isMinimized) {
        QPoint globalPos = QCursor::pos();
        trackedWindow->setGeometry(originalGeometry);
        trackedWindow->setPosition(globalPos.x(), globalPos.y() + 42);
        trackedWindow->show();

        this->resize(originalTopBarWidth, this->height());

        maximizeButton->show();
        closeButton->show();
        minusButton->show();
        resizeButton->show();
        this->setTitle(origTitle);

        MinimizedPosArray::getInstance().freePosition(this->x() - 75);

        isMinimized = false;
        updatePosition();
    }
    
    if (!isResizing) {
        isDragging = true;
        dragStartPos = event->globalPos();
        windowStartPos = trackedWindow->position();
        setCursor(Qt::ClosedHandCursor);
        updatePosition();
        QWidget::mousePressEvent(event);
    }  else {
        QRect windowGeometry = trackedWindow->geometry();
        QPoint bottomRightCorner = windowGeometry.bottomRight();
        QCursor::setPos(bottomRightCorner);
    }
    getTrackedWindow()->raise();
    this->raise();
}

void TopBar::mouseReleaseEvent(QMouseEvent *event) {
    isDragging = false;
    setCursor(Qt::ArrowCursor);
    QWidget::mouseReleaseEvent(event);
    updatePosition();
    if (isResizing) {
        stopResizing();
    }
    getTrackedWindow()->raise();
}

void TopBar::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging) {
        if (isMaximized) {
            trackedWindow->setGeometry(restoreGeometry);
            resizeButton->setVisible(true);
            isMaximized = false;
        }
        getTrackedWindow()->raise();
        this->raise();

        trackedWindow->setPosition(windowStartPos + (event->globalPos() - dragStartPos));
    }

    if (isResizing) {
        QRect windowGeometry = trackedWindow->geometry();
        QPoint currentPos = QCursor::pos();

        int newWidth = windowStartSize.width() + (currentPos.x() - resizeStartPos.x());
        int newHeight = windowStartSize.height() + (currentPos.y() - resizeStartPos.y());

        const int minWidth = 200;
        const int minHeight = 150;

        newWidth = std::max(newWidth, minWidth);
        newHeight = std::max(newHeight, minHeight);

        trackedWindow->setGeometry(trackedWindow->x(), trackedWindow->y(), newWidth, newHeight);
    }

    updatePosition();
    QWidget::mouseMoveEvent(event);
}

void TopBar::closePopup() {
    if (popup->isVisible()) {
        popup->hide();
    }
}

void TopBar::closeTrackedWindow() {
    if (trackedWindow) {
        WId windowId = trackedWindow->winId();

        QProcess process;
        process.start("xdotool getwindowpid " + QString::number(windowId));
        process.waitForFinished();
        QString pidString = process.readAllStandardOutput().trimmed();
        bool ok;
        qint64 pid = pidString.toLongLong(&ok);

        if (ok && pid > 0) {
            if (QProcess::execute("kill -15 " + QString::number(pid)) != 0) {
                QProcess::execute("kill -9 " + QString::number(pid));
            }
        }
        this->close();
    }
}

void TopBar::toggleMaximizeRestore() {
    if (isMaximized) {
        trackedWindow->setGeometry(restoreGeometry);
        resizeButton->setVisible(true);
        isMaximized = false;
    } else {
        restoreGeometry = trackedWindow->geometry();
        maximizeWindow();
        resizeButton->setVisible(false);
        isMaximized = true;
    }

    updatePosition();
}

void TopBar::maximizeWindow() {
     QScreen *screen = QGuiApplication::primaryScreen();
     QRect screenGeometry = screen->availableGeometry();

     int topbarHeight = 36;
     int bottomMargin = 40;

     int newWidth = screenGeometry.width();
     int newHeight = screenGeometry.height() - bottomMargin - topbarHeight;

     trackedWindow->setGeometry(0, topbarHeight, newWidth, newHeight);

     updatePosition();
}

void TopBar::moveMinimizedWindow(bool moveRight) {
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();

    MinimizedPosArray::getInstance().freePosition(this->x());

    int newPosition = moveRight
                        ? MinimizedPosArray::getInstance().getSmallestAvailable() + 38
                        : MinimizedPosArray::getInstance().getSmallestAvailable() - 38;

    if (newPosition < 0) {
        newPosition = 0;
    } else if (newPosition + 50 > screenGeometry.width()) {
        newPosition = screenGeometry.width() - 50;
    }

    int minimizedWidth = 50;

    this->setGeometry(newPosition, screenGeometry.height() - 38, minimizedWidth, 25);
    MinimizedPosArray::getInstance().markPositionAsTaken(newPosition);
}
