#ifndef USERINTERACTRIGHT_H
#define USERINTERACTRIGHT_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QPoint>
#include <QWindow>
#include <QString>
#include <QFile>

class UserInteractRight : public QWidget {
    Q_OBJECT

public:
    explicit UserInteractRight(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void button1Clicked();
    void button2Clicked();

private:
    void setupUI();
    void applyStyles();
    void closeIfClickedOutside(QMouseEvent *event);

    void onWindowClick(QWindow *window);
    void onMouseMove(QMouseEvent *event);
    void onMouseRelease(QMouseEvent *event);

    QPushButton *button1;
    QPushButton *button2;
    QPushButton *button3;
    QLabel *textLabel;
    bool isDarkMode;
    QPoint initialClickPos;


    QString readFileContents(const QString &filePath) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return file.readAll();
        }
        return "Unavailable";
    }

};

#endif // USERINTERACTRIGHT_H
