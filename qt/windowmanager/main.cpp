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
#include <cstdlib>

QFile logFile;

void customLogOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QTextStream out(&logFile);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
        << msg << endl;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    logFile.setFileName("/usr/cydra/logs/a2wm.log");
    logFile.open(QIODevice::Append | QIODevice::Text);

    qInstallMessageHandler(customLogOutput);

    QString platform = QGuiApplication::platformName();

    bool isWayland = std::getenv("WAYLAND_DISPLAY") != nullptr || 
                     (std::getenv("XDG_SESSION_TYPE") && std::string(std::getenv("XDG_SESSION_TYPE")) == "wayland");

    if (platform != "xcb" || isWayland) {
        QMessageBox::critical(nullptr, "Error", "This window manager only works on X11/Xorg.\n\nWhy this error? The variable WAYLAND_DISPLAY is defined or XDG_SESSION_TYPE is set to wayland.\n\n[PRESS ENTER TO CONTINUE]");
        return -1;
    }
    
    WindowManager manager;
    TaskBar taskBar;
    
    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        manager.setGeometry(screenGeometry);
    }

    manager.setWindowTitle("A2WM");
    manager.showFullScreen();
    
    taskBar.show();
    return app.exec();
}
