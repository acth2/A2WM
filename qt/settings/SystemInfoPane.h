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
        layout->setSpacing(2);

        QString wmVersion = readFileContents("/usr/cydra/info/version").trimmed();
        QLabel *wmLabel = createBoldLabel("Window Manager: A2WM (AcTh2WindowManager) - Version: " + wmVersion);
        layout->addWidget(wmLabel);

        QLabel *bootLabel = createBoldLabel("BIOS Boot-Time: " + getBootTime());
        layout->addWidget(bootLabel);

        QLabel *ramLabel = createBoldLabel("Total RAM: " + getTotalRam() + " MB");
        layout->addWidget(ramLabel);

        QStorageInfo storage = QStorageInfo::root();
        QLabel *diskLabel = createBoldLabel("Disk Space Left: " + QString::number(storage.bytesAvailable() / (1024 * 1024 * 1024)) + " GB");
        layout->addWidget(diskLabel);

        QLabel *bitsLabel = createBoldLabel("System Architecture: " + QString::number(QSysInfo::WordSize) + "-bit");
        layout->addWidget(bitsLabel);

        QLabel *cpuLabel = createBoldLabel("Processor: " + getCpuInfo());
        layout->addWidget(cpuLabel);

        QLabel *userLabel = createBoldLabel("Username: " + qgetenv("USER"));
        QLabel *systemLabel = createBoldLabel("System Name: " + QSysInfo::machineHostName());
        layout->addWidget(userLabel);
        layout->addWidget(systemLabel);
    }

private:
    QLabel* createBoldLabel(const QString &text) {
        QLabel *label = new QLabel(text);
        label->setStyleSheet("font-weight: bold; font-size: 14px; margin-bottom: 2px;");
        return label;
    }

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
        return output.isEmpty() ? "Unavailable (Requires root)" : output;
    }

    QString getTotalRam() {
        QFile memInfo("/proc/meminfo");
        if (memInfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString line = memInfo.readLine();
            memInfo.close();
            return line.split(":").last().trimmed().split(" ").first();
        }
        return "Unavailable";
    }

    QString getCpuInfo() {
        QProcess proc;
        proc.start("lscpu");
        proc.waitForFinished();
        QString output = proc.readAllStandardOutput();
        QString cpuName = output.section("Model name:", 1, 1).simplified();
        QString cpuGHz = output.section("CPU MHz:", 1, 1).simplified();
        return cpuName + " " + QString::number(cpuGHz.toDouble() / 1000, 'f', 2) + " GHz";
    }
};
