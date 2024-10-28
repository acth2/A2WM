#include "SettingsApp.h"
#include "SystemInfoPane.h"

SettingsApp::SettingsApp(QWidget *parent) : QMainWindow(parent) {
    if (QFile::exists("/usr/cydra/settings/darkmode")) {
        isDarkMode = true;
    }

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
                min-width: 100px; 
            }
            QPushButton:hover {
                color: #bab9b5;
                background-color: #52514e;
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

    QVBoxLayout *mainLayout = new QVBoxLayout();

    QPushButton *customizationButton = new QPushButton("Customization", this);
    customizationButton->setStyleSheet(buttonStyle);
    mainLayout->addWidget(customizationButton);

    QPushButton *systemInfoButton = new QPushButton("System Information", this);
    systemInfoButton->setStyleSheet(buttonStyle);
    mainLayout->addWidget(systemInfoButton);

    QPushButton *systemSettingsButton = new QPushButton("System Settings", this);
    systemSettingsButton->setStyleSheet(buttonStyle);
    mainLayout->addWidget(systemSettingsButton);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    connect(systemInfoButton, &QPushButton::clicked, this, &SettingsApp::displaySystemInfoPane);
}

void SettingsApp::displaySystemInfoPane() {
    SystemInfoPane *systemInfoPane = new SystemInfoPane(this);
    connect(systemInfoPane, &SystemInfoPane::backRequested, this, &SettingsApp::onBackRequested);
    setCentralWidget(systemInfoPane);
}

void SettingsApp::onBackRequested() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout();

    QPushButton *customizationButton = new QPushButton("Customization", this);
    customizationButton->setStyleSheet(buttonStyle);
    mainLayout->addWidget(customizationButton);

    QPushButton *systemInfoButton = new QPushButton("System Information", this);
    systemInfoButton->setStyleSheet(buttonStyle);
    mainLayout->addWidget(systemInfoButton);

    QPushButton *systemSettingsButton = new QPushButton("System Settings", this);
    systemSettingsButton->setStyleSheet(buttonStyle);
    mainLayout->addWidget(systemSettingsButton);

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    connect(systemInfoButton, &QPushButton::clicked, this, &SettingsApp::displaySystemInfoPane);
}
