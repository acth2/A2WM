#include "taskbar.h"
#include <QtCore/qtextstream.h>
#include <QApplication>
#include <QScreen>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <QMessageBox>
#include <QWidget>
#include <QFile>
#include <QTimer>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

TaskBar::TaskBar(QWidget *parent) : QWidget(parent) {
    if (QFile::exists("/usr/cydra/settings/darkmode")) {
        isDarkMode = true;
    } else {
        setStyleSheet("background-color: #cfcfcf; border: 1px solid #DDDDDD;");
    }
    
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    if (isDarkMode) {
        setStyleSheet("background-color: #333333;");
    }

    startButton = new QPushButton(this);
    if (!isDarkMode) {
        startButton->setIcon(QIcon("/usr/cydra/cydradm.png"));
    } else {
        startButton->setIcon(QIcon("/usr/cydra/cydra.png"));
    }
    startButton->setIconSize(QSize(32, 32));
    startButton->setStyleSheet("border: none;");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(startButton, 0, Qt::AlignLeft | Qt::AlignBottom);
    layout->setContentsMargins(5, 5, 5, 5);

    setLayout(layout);

    popup = new QLabel(nullptr);
    popup->setFixedSize(500, 500);
    if (isDarkMode) {
        popup->setStyleSheet("background-color: #333333; border: 1px solid #000000;");
    } else {
        popup->setStyleSheet("background-color: #fff; border: 1px solid #000000;"); 
    }
    
    popup->hide();

    powerButton = new QPushButton(popup);
    powerButton->setIcon(QIcon("/usr/cydra/icons/power.png"));
    powerButton->setIconSize(QSize(32, 32));
    powerButton->setStyleSheet("border: none;");

    QVBoxLayout *popupLayout = new QVBoxLayout(popup);
    popupLayout->addWidget(powerButton, 0, Qt::AlignBottom | Qt::AlignLeft);
    popupLayout->setContentsMargins(10, 10, 10, 10);

    connect(powerButton, &QPushButton::clicked, this, &TaskBar::showPowerMenu);
    connect(startButton, &QPushButton::clicked, this, &TaskBar::showPopup);

    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    adjustSizeToScreen();
    installEventFilter();
}

void TaskBar::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    adjustSizeToScreen();
}

void TaskBar::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
}

void TaskBar::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape && popup->isVisible()) {
        closePopup();
    }
    QWidget::keyPressEvent(event);
}

void TaskBar::adjustSizeToScreen() {
    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        setFixedSize(screenGeometry.width(), 40);
        move(0, screenGeometry.height() - height());
    }
}

void TaskBar::showPopup() {
    if (isPopupVisible) {
        closePopup();
    } else {
        popup->move(0, height() * 5.7);

        popup->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
        popup->show();

        userIcon->move(popup->x() / 2, popup->y());
        userIcon->setStyleSheet("QLabel { background-color : red; color : blue; }");

        isPopupVisible = true;
    }
}


void TaskBar::closePopup() {
    popup->hide();
    isPopupVisible = false;
}

void TaskBar::showPowerMenu() {
    if (powerMenuVisible) {
        closePowerMenu();
    } else {
        QWidget *overlay = new QWidget(nullptr);
        overlay->setStyleSheet("background: rgba(0, 0, 0, 0.7);");
        overlay->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus);
        overlay->setGeometry(QApplication::primaryScreen()->geometry());
        overlay->show();

        QDialog *powerDialog = new QDialog();
        powerDialog->setWindowTitle("A2WM");
        powerDialog->setModal(true);
        powerDialog->setAttribute(Qt::WA_DeleteOnClose);
        if (isDarkMode) {
        powerDialog->setStyleSheet(R"(
                QDialog {
                    background-color: #2e2e2d;
                    border-radius: 10px;
                }
            )");
        }

        QString buttonStyle;
        QString labelStyle;

        if (isDarkMode) {
                buttonStyle = R"(
                    QPushButton {
                        background-color: #cfcfcf; 
                        color: #595853;
                        border: none;
                        border-radius: 5px; 
                        padding: 8px 16px;
                        font-size: 12px;
                        font-weight: bold;
                        min-width: 100px; width 
                    }
                    QPushButton:hover {
                        color: #bab9b5;
                        background-color: #52514e 
                    }
                )";

                labelStyle = R"(
                    QLabel {
                       color: #bab9b5;
                        font-size: 14px;
                        font-weight: medium;
                        margin-bottom: 10px;
                        background-color: transparent;
                    }
                )";
            } else {
                buttonStyle = R"(
                    QPushButton {
                        background-color: #0078D4;
                        color: white;
                        border: none;
                        border-radius: 5px;
                        padding: 8px 16px;
                        font-size: 12px;
                        font-weight: bold;
                        min-width: 100px; 
                    }
                    QPushButton:hover {
                        background-color: #005A9E;
                    }
                )";

                labelStyle = R"(
                    QLabel {
                        color: #333333;
                        font-size: 14px;
                        font-weight: medium;
                        margin-bottom: 10px;
                        background-color: transparent;
                    }
                )";
        }

        QVBoxLayout *dialogLayout = new QVBoxLayout(powerDialog);
        QLabel *infoLabel = new QLabel("Press ESC to go back", powerDialog);
        infoLabel->setStyleSheet(labelStyle);

        QPushButton *rebootButton = new QPushButton("Reboot", powerDialog);
        rebootButton->setStyleSheet(buttonStyle);

        QPushButton *powerOffButton = new QPushButton("Power Off", powerDialog);
        powerOffButton->setStyleSheet(buttonStyle);

        connect(rebootButton, &QPushButton::clicked, [=]() {
            qApp->exit(1);
        });

        connect(powerOffButton, &QPushButton::clicked, [=]() {
            qApp->exit(0);
        });

        dialogLayout->addWidget(infoLabel);
        dialogLayout->addWidget(rebootButton);
        dialogLayout->addWidget(powerOffButton);

        powerDialog->adjustSize();

        QRect screenGeometry = QApplication::primaryScreen()->geometry();
        int x = (screenGeometry.width() - powerDialog->width()) / 2;
        int y = (screenGeometry.height() - powerDialog->height()) / 2;
        powerDialog->move(x, y);

        connect(powerDialog, &QDialog::finished, [=]() {
            overlay->close();
            overlay->deleteLater();
            powerMenuVisible = false;
        });

        powerDialog->exec();

        powerMenuVisible = true;
    }
}

void TaskBar::closePowerMenu() {
    if (powerMenuVisible) {
        powerMenuVisible = false;
    }
}

void TaskBar::installEventFilter() {
    qApp->installEventFilter(this);
}

bool TaskBar::eventFilter(QObject *object, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (popup->isVisible() && !popup->geometry().contains(mouseEvent->globalPos())) {
            closePopup();
            return true;
        }
    }
    return QWidget::eventFilter(object, event);
}
