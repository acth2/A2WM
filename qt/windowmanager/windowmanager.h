#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <QtCore/qtextstream.h>
#include "win/topbar.h"
#include <QWidget>
#include <QLabel>
#include <QSet>
#include <QTimer>
#include <QMap>
#include <QResizeEvent>
#include <QVector>
#include <QPixmap>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include "taskbar.h"
#include "userinteractright.h"
#include <X11/Xlib.h>

class TopBar;

class UserInteractRight;

class WindowManager : public QWidget {
    Q_OBJECT

public:
    explicit WindowManager(QWidget *parent = nullptr);
    void appendLog(const QString &message);

protected:
    bool event(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void trackWindowEvents(Window xorgWindowId);

private:
    QString backgroundImagePath;
    UserInteractRight *userInteractRightWidget;

    QPoint lastMousePosition;
    void setSupportingWMCheck();

};

#endif // WINDOWMANAGER_H
