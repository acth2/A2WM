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
#include <QFontDatabase>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

TaskBar::TaskBar(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_AlwaysShowToolTips, true);
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

    int fontId = QFontDatabase::addApplicationFont("/usr/cydra/fonts/segoe-ui-semibold.ttf");
    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
    QFont font(fontFamilies.at(0));
    font.setPixelSize(25);

    popup = new QLabel(nullptr);
    popupCenter = new QLabel(nullptr);
    popupExtension = new QLabel(nullptr);
    username = new QLabel(nullptr);
    userLogo = new QPushButton(nullptr);
    username->setText(usernameString);
    userLogo->setIconSize(QSize(126, 126));
    userLogo->setIcon(QIcon("/usr/cydra/icons/usrLogo.png")); 
    userLogo->setStyleSheet(
        "border: none;"
        "background: none;"
        "outline: none;"
        "box-shadow: none;"
    );
    userLogo->setFlat(true);
    popup->setFixedSize(500, 500);
    popupCenter->setFixedSize(380, 375);
    popupCenter->setWindowTitle("A2WM");
    popupExtension->setFixedSize(125, 425);
    username->setFont(font);
    if (isDarkMode) {
        popup->setStyleSheet("background-color: #333333; border: 1px solid #000000;");
        popupCenter->setStyleSheet("background-color: #121212; border: 1px solid #000000;");
        popupExtension->setStyleSheet("background-color: #333333; border: 1px solid #000000;");
        username->setStyleSheet("background-color: #333333");
    } else {
        popup->setStyleSheet("background-color: #fff; border: 1px solid #000000;");
        popupCenter->setStyleSheet("background-color: #989494; border: 1px solid #000000;");
        popupExtension->setStyleSheet("background-color: #fff; border: 1px solid #000000;");
        username->setStyleSheet("background-color: #fff");
    }
    
    popup->hide();
    popupCenter->hide();
    popupExtension->hide();
    userLogo->hide();
    username->hide();

    powerButton = new QPushButton(popup);
    powerButton->setIcon(QIcon("/usr/cydra/icons/power.png"));
    powerButton->setIconSize(QSize(32, 32));
    powerButton->setStyleSheet("border: none;");

    QVBoxLayout *popupLayout = new QVBoxLayout(popup);
    popupLayout->addWidget(powerButton, 0, Qt::AlignBottom | Qt::AlignLeft);
    popupLayout->setContentsMargins(10, 10, 10, 10);

    connect(powerButton, &QPushButton::clicked, this, &TaskBar::showPowerMenu);
    connect(startButton, &QPushButton::clicked, this, &TaskBar::showPopup);

    startButton->setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    startButton->setAttribute(Qt::WA_TransparentForMouseEvents, false);

    powerButton->setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    powerButton->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    
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
        userLogo->move(175, popup->y() * 0.75);
        username->move(userLogo->x() - username->width() - 5, userLogo->y() + userLogo->height() - username->height() * 2);
        popupCenter->move(37, popup->y() + 75);
        popupExtension->move(435, 275);
        popup->show();
        popup->setWindowFlags(windowFlags());
        userLogo->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        popupExtension->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        popupCenter->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        username->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        popupExtension->raise();
        popupCenter->raise();
        userLogo->raise();
        username->raise();
        userLogo->show();
        popupCenter->show();
        username->show();
        popupExtension->show();

        isPopupVisible = true;
    }
}

void TaskBar::closePopup() {
    popup->hide();
    userLogo->hide();
    popupExtension->hide();
    popupCenter->hide();
    username->hide();
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
            if (!userLogo->geometry().contains(mouseEvent->globalPos())) {
                if (!popupExtension->geometry().contains(mouseEvent->globalPos())) {
                    closePopup();
                    return true;
                }
            }
        }
    }
    return QWidget::eventFilter(object, event);
}
