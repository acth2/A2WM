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
#include <QX11Info>
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
    void createAndTrackWindow(WId xorgWindowId, QString windowName = "Unknown window", int width = 0, int height = 0);

protected:
    bool event(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *object, QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void updateTaskbarPosition(QWindow* window);
    void trackWindowEvents(Window xorgWindowId);
    void centerWindow(QWindow *window);
    void removeCloseButton(WId windowId);

private slots:
    void checkForNewWindows();
    void toggleConsole();
    void processX11Events();
    void cleanUpClosedWindows();

private:

    QString backgroundImagePath;
    QLabel *logLabel;
    QSet<QString> loggedMessages;
    KonamiCodeHandler *konamiCodeHandler;
    bool isConsoleVisible;
    UserInteractRight *userInteractRightWidget;

    void listExistingWindows();
    QMap<WId, QWindow*> trackedWindows;
    QMap<QWindow*, TaskBar*> windowTaskbars;
    QTimer *windowCheckTimer;
    QTimer *resizeWindowCubesTimer;
    struct TrackingSquares {
        QLabel *leftSquare;
        QLabel *rightSquare;
        QLabel *bottomSquare;
    };
    QMap<WId, TrackingSquares> windowSquares;
    bool resizeMode;
    QPoint lastMousePosition;

    void setupCloseButton(QWindow *window);
    void setSupportingWMCheck();


    QRect *windowGeometry;
    QMap<WId, QWidget*> trackedContainers;
    QString nameExtractor;

    bool isGraphicalWindow(Window window, int& width, int& height);
    void handleWindowVisibilityChange(Window window, bool isVisible);
    QString getWindowName(Display xDisplay, Window window);
    Display* display;
};

#endif // WINDOWMANAGER_H
