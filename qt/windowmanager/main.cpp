#include "windowmanager.h"
#include "taskbar.h"
#include "utils/autoRefresh.h"
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
        << msg << endl;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    logFile.setFileName("/usr/cydra/logs/a2wm.log");
    logFile.open(QIODevice::Append | QIODevice::Text);

    qInstallMessageHandler(customLogOutput);

    QString platform = QGuiApplication::platformName();
    if (!QX11Info::isPlatformX11()) {
        QMessageBox::critical(nullptr, "Error", "This window manager only works on X11/Xorg.\nPlease restart A2WM with X11\n\n[PRESS ENTER TO CONTINUE]");
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
