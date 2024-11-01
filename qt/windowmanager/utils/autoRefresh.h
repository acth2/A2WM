#include <QApplication>
#include <QTimer>
#include <QProcess>
#include <QScreen>
#include <QDebug>

class DisplayRefresher : public QObject {
    Q_OBJECT

public:
    DisplayRefresher(QObject *parent = nullptr) : QObject(parent) {
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &DisplayRefresher::refreshDisplay);
        timer->start(1000);
    }

private slots:
    void refreshDisplay() {
        QString display = getCurrentDisplay();

        if (!display.isEmpty()) {
            QProcess::execute("xrefresh", QStringList() << "-display" << display);
        } else {
            qWarning() << "Unable to determine the current display number.";
        }
    }

private:
    QString getCurrentDisplay() {
        QByteArray displayEnv = qgetenv("DISPLAY");
        return !displayEnv.isEmpty() ? QString(displayEnv) : QString();
    }
};
