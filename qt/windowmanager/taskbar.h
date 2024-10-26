#ifndef TASKBAR_H
#define TASKBAR_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDialog>
#include <QEvent>
#include <cstdlib>

class TaskBar : public QWidget {
    Q_OBJECT

public:
    explicit TaskBar(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *event) override;
    void showPopup();
    void closePopup();
    bool isPopupVisible = false;
    QString getFormattedDirectories();

signals:
    void windowMinimized();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QLabel *popup;
    QLabel *popupCenter;
    QLabel *popupExtension;
    QLabel *username;
    QString usernameString = QString::fromLocal8Bit(getenv("USER"));
    QPushButton *userLogo;
    QPushButton *startButton;
    QPushButton *powerButton;
    void adjustSizeToScreen();
    void showPowerMenu();
    void closePowerMenu();
    void installEventFilter();
    bool powerMenuVisible = false;
    bool isDarkMode;
    bool isWindowVisible = true;
    bool firstTime = true;

    void onLabelClicked(const QString &labelText);
    void onLabelClickedExec(const QString &execCommand);
};

#endif // TASKBAR_H
