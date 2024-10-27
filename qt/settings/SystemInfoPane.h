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
        QLabel *wmLabel = new QLabel("Window Manager: A2WM (AcTh2WindowManager) - Version: " + wmVersion);
        layout->addWidget(wmLabel);

        QLabel *bootLabel = new QLabel("BIOS Boot-Time: " + getBootTime());
        layout->addWidget(bootLabel);

        QLabel *ramLabel = new QLabel("Total RAM: " + getTotalRam() + " MB");
        layout->addWidget(ramLabel);

        QStorageInfo storage = QStorageInfo::root();
        QLabel *diskLabel = new QLabel("Disk Space Left: " + QString::number(storage.bytesAvailable() / (1024 * 1024 * 1024)) + " GB");
        layout->addWidget(diskLabel);

        QLabel *bitsLabel = new QLabel("System Architecture: " + QString::number(QSysInfo::WordSize) + "-bit");
        layout->addWidget(bitsLabel);

        QLabel *cpuLabel = new QLabel("Processor: " + getCpuInfo());
        layout->addWidget(cpuLabel);

        QLabel *userLabel = new QLabel("Username: " + qgetenv("USER"));
        QLabel *systemLabel = new QLabel("System Name: " + QSysInfo::machineHostName());
        layout->addWidget(userLabel);
        layout->addWidget(systemLabel);
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
        return output.isEmpty() ? "Unavailable" : output;
    }

    QString getTotalRam() {
        QFile file("/proc/meminfo");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            while (!file.atEnd()) {
                QString line = file.readLine();
                if (line.startsWith("MemTotal:")) {
                    QStringList parts = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
                    if (parts.size() >= 2) {
                        return QString::number(parts[1].toInt() / 1024);
                    }
                }
            }
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
};
