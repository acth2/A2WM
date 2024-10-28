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
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);

        QString wmVersion = readFileContents("/usr/cydra/info/version").trimmed();
        QLabel *wmLabel = createLabel("Window Manager: A2WM (AcTh2WindowManager) - Version: " + wmVersion);
        layout->addWidget(wmLabel);

        QLabel *bootLabel = createLabel("BIOS Boot-Time: " + getBootTime());
        layout->addWidget(bootLabel);

        layout->addWidget(createBoldLabel("Total RAM: " + getTotalRam() + " MB"));

        QStorageInfo storage = QStorageInfo::root();
        QLabel *diskLabel = createLabel("Disk Space Left: " + QString::number(storage.bytesAvailable() / (1024 * 1024 * 1024)) + " GB");
        layout->addWidget(diskLabel);

        QLabel *bitsLabel = createLabel("System Architecture: " + QString::number(QSysInfo::WordSize) + "-bit");
        layout->addWidget(bitsLabel);

        QLabel *cpuLabel = createLabel("Processor: " + getShortCpuInfo());
        layout->addWidget(cpuLabel);

        QLabel *userLabel = createLabel("Username: " + qgetenv("USER"));
        QLabel *systemLabel = createLabel("System Name: " + QSysInfo::machineHostName());
        layout->addWidget(userLabel);
        layout->addWidget(systemLabel);
    }

private:
    QLabel* createBoldLabel(const QString &text) {
        QLabel *label = new QLabel(text);
        label->setStyleSheet("font-weight: bold; font-size: 14px; margin-bottom: 1px;");
        return label;
    }

    QLabel* createLabel(const QString &text) {
        QLabel *label = new QLabel("<b>" + text + "</b>");
        label->setStyleSheet("font-size: 14px;");
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
        return output.isEmpty() ? "Unavailable" : output;
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

    QString getShortCpuInfo() {
        QProcess proc;
        proc.start("lscpu");
        proc.waitForFinished();
        QString output = proc.readAllStandardOutput();
        QString cpuInfo = output.section("Model name:", 1, 1).trimmed();
        return cpuInfo.section(' ', 0, 2);
    }
};
