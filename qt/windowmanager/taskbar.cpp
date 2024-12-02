#include "taskbar.h"
#include "utils/ClickableLabel.h"
#include <QtCore/qtextstream.h>
#include <QApplication>
#include <QMessageBox>
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
#include <QDir>
#include <QDebug>
#include <QProcess>
#include <QList>
#include <QWindow>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <regex>
#include <QRegularExpression>
#include <QX11Info>
#include <QWindowStateChangeEvent>
#include <QDateTime>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

QMap<WId, QPushButton*> minimizedWindows;
QList<QWindow *> openWindows;
TaskBar::TaskBar(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_AlwaysShowToolTips, true);
    this->setWindowTitle("A2WM");
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
    
    if (QX11Info::isPlatformX11()) {
        Display* display = QX11Info::display();

        Atom belowAtom = XInternAtom(display, "_NET_WM_STATE_BELOW", False);
        Atom stateAtom = XInternAtom(display, "_NET_WM_STATE", False);
        XChangeProperty(display, winId(), stateAtom, XA_ATOM, 32,
                        PropModeReplace, reinterpret_cast<unsigned char*>(&belowAtom), 1);

        Atom dockAtom = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DOCK", False);
        Atom typeAtom = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
        XChangeProperty(display, winId(), typeAtom, XA_ATOM, 32,
                        PropModeReplace, reinterpret_cast<unsigned char*>(&dockAtom), 1);
    }

    QVBoxLayout *timeDateLayout = new QVBoxLayout(timeDateContainer);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(startButton, 0, Qt::AlignLeft | Qt::AlignBottom);
    layout->setContentsMargins(5, 5, 5, 5);

    setLayout(layout);

    dateLabel = new QLabel(this);
    int fontId = QFontDatabase::addApplicationFont("/usr/cydra/fonts/segoe-ui-semibold.ttf");
    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
    QFont font(fontFamilies.at(0));
    font.setPixelSize(25);
    
    timeDateLayout->addWidget(dateLabel);

    timeDateContainer = new QWidget(this);
    
    timeTimer = new QTimer(this);
    connect(timeTimer, &QTimer::timeout, this, &TaskBar::updateTime);
    timeTimer->start(1000);

    QFont slimFont("Arial", 10, QFont::Light);
    dateLabel->setFont(slimFont);

    layout->addWidget(dateLabel, 1, Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(timeDateContainer, 0, Qt::AlignRight | Qt::AlignVCenter);
    layout->setContentsMargins(5, 5, 5, 5);

    popup = new QLabel(nullptr);
    popupCenter = new QLabel(nullptr);
    popupExtension = new QLabel(nullptr);
    username = new QLabel(nullptr);
    userLogo = new QPushButton(nullptr);
    username->setText(usernameString);
    userLogo->setIconSize(QSize(126, 126));
    userLogo->setStyleSheet("background-color: transparent; border: none;");
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
    popup->setWindowTitle("A2WM");
    popupExtension->setWindowTitle("A2WM");
    userLogo->setWindowTitle("A2WM");
    username->setWindowTitle("A2WM");
    popupExtension->setFixedSize(125, 425);
    username->setFont(font);
    if (isDarkMode) {
        popup->setStyleSheet("background-color: #333333; border: 1px solid #000000;");
        popupCenter->setStyleSheet("background-color: #1F1F1F; border: 1px solid #000000;");
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

QString TaskBar::getFormattedDirectories() {
    QString homeDir = QDir::homePath() + "/.a2wm/startMenu";
    QDir dir(homeDir);
    QStringList formattedDirectories;

    if (dir.exists()) {
        QStringList directories = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        popupExtension->setText("");
        if (popupExtension->layout()) {
            QLayoutItem *item;
            while ((item = popupExtension->layout()->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            delete popupExtension->layout();
        }

        QVBoxLayout *layout = new QVBoxLayout(popupExtension);

        for (const QString &dirName : directories) {
            QString displayName = dirName.length() > 10 ? dirName.left(10) + "-" : dirName;
            ClickableLabel *label = new ClickableLabel(displayName, homeDir + "/" + dirName, popupExtension);
            connect(label, &ClickableLabel::clicked, this, [this, dirName]() {
                QLayout* layout = popupCenter->layout();
                if (layout) {
                    QLayoutItem* item;
                    while ((item = layout->takeAt(0)) != nullptr) {
                        delete item->widget();
                        delete item;
                    }
                    delete layout;
                }
                onLabelClicked(dirName);
            });
            label->setAlignment(Qt::AlignCenter);
            layout->addWidget(label);
        }
        
        popupExtension->setLayout(layout);
    }

    int fontId = QFontDatabase::addApplicationFont("/usr/cydra/fonts/segoe-ui-semibold.ttf");
    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);

    if (!fontFamilies.isEmpty()) {
        QFont font(fontFamilies.at(0));
        font.setPointSize(16);
        popupExtension->setFont(font);
    } else {
        qDebug() << "Font not loaded. Please check the path.";
    }

    popupExtension->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    return formattedDirectories.join("\n");
}

void TaskBar::onLabelClickedExec(const QString &execCommand) {
    QProcess *process = new QProcess(this);
    process->start(execCommand);
    closePopup();
    if (!process->waitForStarted()) {
        QMessageBox::warning(popupCenter, "A2WM", 
                             "Failed to start application: " + execCommand);
        delete process;
    }
}

void TaskBar::onLabelClicked(const QString &labelText) {
    if (popupCenter->layout()) {
        QLayoutItem *item;
        while ((item = popupCenter->layout()->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete popupCenter->layout();
    }

    QGridLayout *layout = new QGridLayout(popupCenter);
    layout->setSpacing(5);
    layout->setContentsMargins(0, 0, 0, 0);

    QDir directory(QString("/home/%1/.a2wm/startMenu/%2").arg(getenv("USER")).arg(labelText));
    std::cout << "Accessing directory: " << directory.absolutePath().toStdString() << '\n';

    QRegularExpression execRegex(R"(Exec=(.*))");
    QRegularExpression nameRegex(R"(Name=(.*))");

    if (!directory.exists()) {
        std::cerr << "Directory does not exist: " << directory.absolutePath().toStdString() << '\n';
        return;
    }

    QStringList execList;
    QStringList desktopFiles = directory.entryList(QStringList() << "*.desktop", QDir::Files);
    std::cout << "Found desktop files: " << desktopFiles.join(", ").toStdString() << '\n';

    int row = 0;
    int col = 0;
    const int maxCols = 5;

    for (const QString &fileName : desktopFiles) {
        QFile file(directory.filePath(fileName));

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            std::cerr << "Failed to open file: " << file.fileName().toStdString() << '\n';
            continue;
        }

        QTextStream in(&file);
        QString execValue, nameValue;
        while (!in.atEnd()) {
            QString line = in.readLine();
            QRegularExpressionMatch execMatch = execRegex.match(line);
            QRegularExpressionMatch nameMatch = nameRegex.match(line);

            if (execMatch.hasMatch()) {
                execValue = execMatch.captured(1).trimmed();
            } else if (nameMatch.hasMatch()) {
                nameValue = nameMatch.captured(1).trimmed();
            }
        }
        file.close();

        std::cout << "File: " << fileName.toStdString() << ", Name: " << nameValue.toStdString() << ", Exec: " << execValue.toStdString() << '\n';

        if (!nameValue.isEmpty()) {
            ClickableLabel *label = new ClickableLabel(nameValue, directory.filePath(fileName), popupCenter);
            label->setAlignment(Qt::AlignCenter);
            label->setFixedSize(64, 64);
            layout->addWidget(label, row, col, Qt::AlignTop | Qt::AlignLeft);
            execList.append(execValue);

            connect(label, &ClickableLabel::clicked, this, [this, execValue]() {
                onLabelClickedExec(execValue);
                closePopup();
            });

            if (++col >= maxCols) {
                col = 0;
                ++row;
            }
        }
    }

    popupCenter->setLayout(layout);
    popupCenter->adjustSize();
    popupCenter->setVisible(true);

    for (const QString &exec : execList) {
        std::cout << "Stored Exec: " << exec.toStdString() << '\n';
    }
}


void TaskBar::showPopup() {
    if (isPopupVisible) {
        closePopup();
    } else {
        QString directoryText = getFormattedDirectories();
        popupExtension->setText(directoryText);
        popupExtension->setWordWrap(true);
        
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();

        setFixedSize(screenGeometry.width(), 40);
        popup->move(0, screenGeometry.height() - popup->height() - 40);
        
        //userLogo->move(175, popup->y() * 0.75);
        //username->move(userLogo->x() - username->width() - 5, userLogo->y() + userLogo->height() - username->height() * 2);
        //popupCenter->move(37, popup->y() + 75);
        popupExtension->move(popup->width() + popupExtension->width() / 2, popup->height() + popup->height() / 6);

        popup->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        userLogo->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        popupExtension->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        popupCenter->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        username->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

        popup->raise();
        popupExtension->raise();
        popupCenter->raise();
        userLogo->raise();
        username->raise();

        popup->show();
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

void TaskBar::updateTime() {
        QLocale french(QLocale::French);
        QString currentDate = french.toString(QDate::currentDate(), "dddd dd MMMM yyyy");
        dateLabel->setText(currentDate);
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
