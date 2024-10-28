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
        layout->setSpacing(1);

        QString wmVersion = readFileContents("/usr/cydra/info/version").trimmed();
        layout->addWidget(createBoldLabel("Window Manager: A2WM (AcTh2WindowManager) - Version: " + wmVersion));

        layout->addWidget(createBoldLabel("BIOS Boot-Time: " + getBootTime()));
        
        layout->addWidget(createBoldLabel("Total RAM: " + getTotalRam() + " MB"));

        QStorageInfo storage = QStorageInfo::root();
        layout->addWidget(createBoldLabel("Disk Space Left: " + QString::number(storage.bytesAvailable() / (1024 * 1024 * 1024)) + " GB"));

        layout->addWidget(createBoldLabel("System Architecture: " + QString::number(QSysInfo::WordSize) + "-bit"));

        layout->addWidget(createBoldLabel("Processor: " + getCpuInfo()));

        layout->addWidget(createBoldLabel("Username: " + qgetenv("USER")));
        layout->addWidget(createBoldLabel("System Name: " + QSysInfo::machineHostName()));
    }

private:
    QLabel* createBoldLabel(const QString &text) {
        QLabel *label = new QLabel(text);
        label->setStyleSheet("font-weight: bold; font-size: 14px; margin-bottom: 1px;");
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
            bool ok;
            double kb = line.split(":").last().trimmed().split(" ").first().toDouble(&ok);
            if (ok) return QString::number(kb / 1024, 'f', 0);
        }
        return "Unavailable";
    }

    QString getCpuInfo() {
        QProcess proc;
        proc.start("lscpu");
        proc.waitForFinished();
        QString output = proc.readAllStandardOutput();
        QString cpuName = output.section("Model name:", 1, 1).simplified();
        QString cpuMHz = output.section("CPU MHz:", 1, 1).simplified();
        if (!cpuMHz.isEmpty()) {
            double ghz = cpuMHz.toDouble() / 1000;
            return cpuName + " (" + QString::number(ghz, 'f', 2) + " GHz)";
        }
        return cpuName;
    }
};
