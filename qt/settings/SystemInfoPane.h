#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QFile>
#include <QProcess>
#include <QSysInfo>
#include <QStorageInfo>
#include <QPushButton>
#include <sys/utsname.h>

class SystemInfoPane : public QWidget {
    Q_OBJECT
public:
    SystemInfoPane(QWidget *parent = nullptr) : QWidget(parent) {
        if (QFile::exists("/usr/cydra/settings/darkmode")) {
            isDarkMode = true;
            this->setStyleSheet("background-color: rgb(30, 30, 30);");
        }
    
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setSpacing(1);

        QPushButton *backButton = new QPushButton("Back", this);
        backButton->setStyleSheet(getButtonStyle());
        layout->addWidget(backButton);

        connect(backButton, &QPushButton::clicked, this, &SystemInfoPane::onBackButtonClicked);

        QString wmVersion = readFileContents("/usr/cydra/info/version").trimmed();
        layout->addWidget(createBoldLabel("Window Manager: A2WM Version: " + wmVersion));
        layout->addWidget(createBoldLabel("BIOS Boot-Time: " + getBootTime()));
        layout->addWidget(createBoldLabel("Total RAM: " + getTotalRam() + " MB"));

        QStorageInfo storage = QStorageInfo::root();
        layout->addWidget(createBoldLabel("Disk Space Left: " + QString::number(storage.bytesAvailable() / (1024 * 1024 * 1024)) + " GB"));

        layout->addWidget(createBoldLabel("System Architecture: " + QString::number(QSysInfo::WordSize) + "-bit"));
        layout->addWidget(createBoldLabel("Processor: " + getCpuInfo()));
        layout->addWidget(createBoldLabel("Username: " + qgetenv("USER")));
        layout->addWidget(createBoldLabel("System Name: " + QSysInfo::machineHostName()));
        layout->addWidget(createBoldLabel("OS Name: " + QSysInfo::prettyProductName()));
        layout->addWidget(createBoldLabel("Kernel Version: " + getKernelVersion()));

        layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    }

private slots:
    void onBackButtonClicked() {
        emit backRequested();
    }

private:
    bool isDarkMode = false;

    QLabel* createBoldLabel(const QString &text) {
        QLabel *label = new QLabel(text);
        QString labelStyle = isDarkMode ? 
            "QLabel { color: white; font-size: 14px; font-weight: medium; margin-bottom: 10px; background-color: transparent; }" :
            "QLabel { color: #333333; font-size: 14px; font-weight: medium; margin-bottom: 10px; background-color: transparent; }";
        label->setStyleSheet("font-weight: bold; font-size: 14px; margin-bottom: 1px;");
        label->setStyleSheet(labelStyle);
        return label;
    }

    QString getButtonStyle() const {
        if (QFile::exists("/usr/cydra/settings/darkmode")) {
            return R"(
                QPushButton {
                    background-color: #cfcfcf; 
                    color: #595853;
                    border: none;
                    border-radius: 5px; 
                    padding: 8px 16px;
                    font-size: 12px;
                    font-weight: bold;
                    min-width: 100px; 
                }
                QPushButton:hover {
                    color: #bab9b5;
                    background-color: #52514e;
                }
            )";
        } else {
            return R"(
                QPushButton {
                    background-color: #0078D4;
                    color: white;
                    border: none;
                    border-radius: 5px;
                    padding: 8px 16px;
                    font-size: 12px;
                    font-weight: bold;
                    min-width: 100px;
                }
                QPushButton:hover {
                    background-color: #005A9E;
                }
            )";
        }
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
        cpuName = cpuName.split('@').first().trimmed();
        QString cpuMHz = output.section("CPU MHz:", 1, 1).simplified();
        if (!cpuMHz.isEmpty()) {
            double ghz = cpuMHz.toDouble() / 1000;
            return cpuName + " (" + QString::number(ghz, 'f', 2) + " GHz)";
        }
        return cpuName;
    }

    QString getKernelVersion() {
        struct utsname buffer;
        if (uname(&buffer) == 0) {
            return QString(buffer.sysname) + " " + QString(buffer.release);
        }
        return "Unavailable";
    }

signals:
    void backRequested();
};
