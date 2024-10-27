#include "SettingsApp.h"

SettingsApp::SettingsApp(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void SettingsApp::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);

    customizationButton = new QPushButton("Customization", this);
    systemInfoButton = new QPushButton("System Information", this);
    systemSettingsButton = new QPushButton("System Settings", this);

    QString buttonStyle = R"(
        QPushButton {
            background-color: #4CAF50;
            color: white;
            font-size: 16px;
            padding: 15px;
            border: none;
            border-radius: 10px;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
    )";
    customizationButton->setStyleSheet(buttonStyle);
    systemInfoButton->setStyleSheet(buttonStyle);
    systemSettingsButton->setStyleSheet(buttonStyle);

    layout->addWidget(customizationButton);
    layout->addWidget(systemInfoButton);
    layout->addWidget(systemSettingsButton);
    layout->setSpacing(15);
    layout->setContentsMargins(30, 30, 30, 30);

    setLayout(layout);
}
