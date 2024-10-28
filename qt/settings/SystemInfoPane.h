#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QFile>
#include <QProcess>
#include <QSysInfo>
#include <QStorageInfo>
#include <QStandardPaths>

class SystemInfoPane : public QWidget {
    Q_OBJECT
public:
    SystemInfoPane(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *layout = new QVBoxLayout(this);

        QString wmVersion = readFileContents("/usr/cydra/info/version").trimmed();
        addStyledLabel(layout, "Window Manager:", "A2WM (AcTh2WindowManager) - Version: " + wmVersion);

        QString bootTime = getBootTime();
        addStyledLabel(layout, "BIOS Boot-Time:", bootTime);

        QString ramSize = getTotalRAM();
        addStyledLabel(layout, "Total RAM:", ramSize + " MB");

        QStorageInfo storage = QStorageInfo::root();
        addStyledLabel(layout, "Disk Space Left:", QString::number(storage.bytesAvailable() / (1024 * 1024 * 1024)) + " GB");

        addStyledLabel(layout, "System Architecture:", QString::number(QSysInfo::WordSize) + "-bit");
        addStyledLabel(layout, "Processor:", getCpuInfo());

        QString username = qgetenv("USER");
        QString systemName = QSysInfo::machineHostName();
        addStyledLabel(layout, "Username:", username);
        addStyledLabel(layout, "System Name:", systemName);
    }

private:
    QString readFileContents(const QString &filePath) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return file.readAll();
        }
        return "Unavailable";
    }

    QString getBootTime() {
        QProcess proc;
        proc.start("systemd-analyze", QStringList() << "time");
        proc.waitForFinished();
        QString output = proc.readAllStandardOutput();
        return output.isEmpty() ? "Root permissions required" : output;
    }

    QString getTotalRAM() {
        QFile memInfo("/proc/meminfo");
        if (memInfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString content = memInfo.readLine();
            return content.section(" ", -2, -2).trimmed();
        }
        return "Unavailable";
    }

    QString getCpuInfo() {
        QProcess proc;
        proc.start("lscpu");
        proc.waitForFinished();
        QString output = proc.readAllStandardOutput();
        return output.section("Model name:", 1, 1).trimmed();
    }

    void addStyledLabel(QVBoxLayout *layout, const QString &labelText, const QString &valueText) {
        QLabel *label = new QLabel("<b>" + labelText + "</b> " + valueText, this);
        label->setStyleSheet("font-size: 16px; margin-bottom: 6px;");
        layout->addWidget(label);
    }
};
