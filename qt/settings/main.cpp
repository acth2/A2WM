#include <QApplication>
#include "SettingsApp.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    SettingsApp settings;
    settings.setWindowTitle("A2WM Settings");
    settings.resize(400, 300);
    settings.show();
    return app.exec();
}
