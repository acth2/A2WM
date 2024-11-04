#ifndef TOPBAR_H
#define TOPBAR_H

#include <iostream> 
#include <QWidget>
#include <QWindow>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QProcess>
#include <QPaintEvent>
#include <QEvent>
#include <QObject>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QRect>
#include <win/MinimizedPosArray.h>

class WindowManager;

class TopBar : public QWidget {
    Q_OBJECT

public:
    explicit TopBar(QWindow *parentWindow, WindowManager *manager, QWidget *parent = nullptr);
    void updatePosition();
    void setTitle(const QString &title);
    QWindow* getTrackedWindow() const;
    QLabel* getPopup() const;
    void closePopup();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    void closeRequested();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void closeTrackedWindow();
    void maximizeWindow();
    void startResizing();
    void stopResizing();
    void toggleMaximizeRestore();
    void minimizeWindow();
    void onLoop();
    void moveMinimizedWindow(bool moveRight);

private:
    QLabel *titleLabel;
    QLabel *popup;
    QPushButton *closeButton;
    QPushButton *maximizeButton;
    QPushButton *resizeButton;
    QPushButton *minusButton;
    bool isDragging = false;
    QPoint dragStartPos;
    QPoint windowStartPos;
    WindowManager *windowManager;
    QWindow *trackedWindow;
    QRect restoreGeometry;
    bool isMaximized;

    bool isDarkMode;
    bool atomicStopper = false;

    bool isResizing;
    QPoint resizeStartPos;
    QSize windowStartSize;

    int minimalSizePos;
    bool isMinimized = false;

    QRect originalGeometry;
    std::vector<int> minimizedSlots;
    int originalTopBarWidth;
    QSize originalTopBarSize;

    QString origTitle;
};

#endif // TOPBAR_H
