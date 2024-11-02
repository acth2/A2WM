#include "userinteractright.h"
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QApplication>
#include <QFile>
#include <QProcess>
#include <QDebug>
#include <QCursor>
#include <QScreen>
#include <QWindow>

UserInteractRight::UserInteractRight(QWidget *parent) 
    : QWidget(parent), isDarkMode(false) {

    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setupUI();

    setFixedSize(200, 250);

    setWindowTitle("A2WM");

    if (QFile::exists("/usr/cydra/settings/darkmode")) {
        isDarkMode = true;
    }
    
    applyStyles();

    qApp->installEventFilter(this);
}

void UserInteractRight::setupUI() {
    button1 = new QPushButton(readFileContents("/usr/cydra/info/CPMB1").trimmed(), this);
    button2 = new QPushButton(readFileContents("/usr/cydra/info/CPMB2").trimmed(), this);
    button3 = new QPushButton(readFileContents("/usr/cydra/info/CPMB3").trimmed(), this);
    textLabel = new QLabel(readFileContents("/usr/cydra/info/CPL").trimmed(), this);

    textLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(textLabel);
    layout->addWidget(button1);
    layout->addWidget(button2);
    layout->addWidget(button3);
    layout->setSpacing(10); 
    layout->setContentsMargins(15, 15, 15, 15);
    setLayout(layout);

    connect(button1, &QPushButton::clicked, this, &UserInteractRight::button1Clicked);
    connect(button2, &QPushButton::clicked, this, &UserInteractRight::button2Clicked);
    connect(button3, &QPushButton::clicked, this, &UserInteractRight::button3Clicked);
}

void UserInteractRight::applyStyles() {
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

    textLabel->setStyleSheet(labelStyle);
    button1->setStyleSheet(buttonStyle);
    button2->setStyleSheet(buttonStyle);
    button3->setStyleSheet(buttonStyle);
}

void UserInteractRight::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        QPoint cursorPos = event->globalPos();
        move(cursorPos.x() - width() / 2, cursorPos.y() - height() / 2);
        show();
    }
    
    QWidget::mousePressEvent(event);
}

void UserInteractRight::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        closeIfClickedOutside(event);
    }
    QWidget::mouseReleaseEvent(event);
}

void UserInteractRight::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.setPen(QPen(Qt::black, 2)); 
    if (isDarkMode) {
        painter.setBrush(QColor(31, 30, 29, 240)); 
    } else {
        painter.setBrush(QColor(255, 255, 255, 240)); 
    }
    painter.drawRect(rect());

    QWidget::paintEvent(event);
}

void UserInteractRight::button1Clicked() {
    QProcess *xtermProcess = new QProcess(this);

    QString program = readFileContents("/usr/cydra/info/CPBA1").trimmed();
    QStringList arguments;

    xtermProcess->start(program, arguments);

    connect(xtermProcess, &QProcess::errorOccurred, [](QProcess::ProcessError error) {
        qDebug() << "Error occurred:" << error;
    });
    close();
}

void UserInteractRight::button2Clicked() {
    QProcess *xtermProcess2 = new QProcess(this);

    QString program2 = readFileContents("/usr/cydra/info/CPBA2").trimmed();
    QStringList arguments2;

    xtermProcess2->start(program2, arguments2);

    connect(xtermProcess2, &QProcess::errorOccurred, [](QProcess::ProcessError error) {
        qDebug() << "Error occurred:" << error;
    });
    close();
}

void UserInteractRight::button3Clicked() {
    QProcess *xtermProcess2 = new QProcess(this);

    QString program2 = readFileContents("/usr/cydra/info/CPBA3").trimmed();
    QStringList arguments2;

    xtermProcess2->start(program2, arguments2);

    connect(xtermProcess2, &QProcess::errorOccurred, [](QProcess::ProcessError error) {
        qDebug() << "Error occurred:" << error;
    });
    close();
}


void UserInteractRight::closeIfClickedOutside(QMouseEvent *event) {
    if (!rect().contains(mapFromGlobal(event->globalPos()))) {
        close();
    }
}
