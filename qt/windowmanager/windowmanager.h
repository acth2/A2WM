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
#include "konami_code_handler.h"
#include "userinteractright.h"
#include <X11/Xlib.h>

class TopBar;

class UserInteractRight;

class WindowManager : public QWidget {
    Q_OBJECT

public:
    explicit WindowManager(QWidget *parent = nullptr);
    void appendLog(const QString &message);
    QMap<WId, TopBar*> windowTopBars;
    void closeWindow(WId xorgWindowId);
    void resizeTrackedWindow(WId xorgWindowId, int newWidth, int newHeight);

protected:
    bool event(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *object, QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void trackWindowEvents(Window xorgWindowId);
    void centerWindow(QWindow *window);
    void removeCloseButton(WId windowId);

private slots:
    void toggleConsole();

private:

    QString backgroundImagePath;
    QLabel *logLabel;
    QSet<QString> loggedMessages;
    KonamiCodeHandler *konamiCodeHandler;
    bool isConsoleVisible;
    UserInteractRight *userInteractRightWidget;

    QPoint lastMousePosition;

    void setupCloseButton(QWindow *window);
    void setSupportingWMCheck();

    QRect *windowGeometry;
};

#endif // WINDOWMANAGER_H
