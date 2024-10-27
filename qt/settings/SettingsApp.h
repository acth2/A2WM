#ifndef SETTINGSAPP_H
#define SETTINGSAPP_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QFile>
#include <QVBoxLayout>

class SettingsApp : public QMainWindow {
    Q_OBJECT

public:
    SettingsApp(QWidget *parent = nullptr);

private:
    bool isDarkMode = false;
    QString buttonStyle;
    QString labelStyle;
};

#endif // SETTINGSAPP_H
