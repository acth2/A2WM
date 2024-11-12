#include "windowmanager.h"
#include "taskbar.h"
#include <QApplication>
#include <QScreen>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QtMessageHandler>
#include <QMessageBox>
#include <QGuiApplication>
#include <QX11Info>
#include <cstdlib>

QFile logFile;

void customLogOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QTextStream out(&logFile);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
        << msg << Qt::endl;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    logFile.setFileName("/usr/cydra/logs/a2wm.log");
    logFile.open(QIODevice::Append | QIODevice::Text);

    qInstallMessageHandler(customLogOutput);

    // Check if the window manager is booted with Xorg
    QString platform = QGuiApplication::platformName();
    if (!QX11Info::isPlatformX11()) {
        QMessageBox::critical(nullptr, "Error", "This window manager only works on X11/Xorg.\nPlease restart A2WM with X11\n\n[PRESS ENTER TO CONTINUE]");
        return -1;
    }
    
    WindowManager manager;
    TaskBar taskBar;

    // Create the wallpaper
    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        manager.setGeometry(screenGeometry);
    }

    manager.setWindowTitle("A2WM");
    manager.showFullScreen();

    // Create the taskbar
    taskBar.show();
    return app.exec();
}
