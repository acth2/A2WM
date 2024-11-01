#include <QDir>
#include <QFileInfo>
#include <QGridLayout>
#include <QPushButton>
#include <QWidget>

class DesktopIcons : public QWidget {
public:
    DesktopIcons(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("A2WM");
        setWindowFlag(Qt::FramelessWindowHint);

        QGridLayout *layout = new QGridLayout(this);
        
        QDir dir(QDir::homePath() + "/.a2wm/desktop");
        int row = 0, col = 0;
        
        foreach (QFileInfo item, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
            QPushButton *iconButton = new QPushButton(this);
            QString iconPath = item.isDir() ? "/usr/cydra/icons/dir.png" : "/usr/cydra/icons/file.png";
            iconButton->setIcon(QIcon(iconPath));
            iconButton->setIconSize(QSize(64, 64));
            
            iconButton->setStyleSheet("border: none;");
            
            layout->addWidget(iconButton, row, col);
            col++;
            if (col >= 5) {
                col = 0;
                row++;
            }
        }
        setLayout(layout);
    }
};
