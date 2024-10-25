#include <QLabel>
#include <QWidget>
#include <QMouseEvent>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(const QString &text, const QString &dirPath, QWidget *parent = nullptr)
        : QLabel(text, parent), directoryPath(dirPath) {}

    QString getDirectoryPath() const { return directoryPath; }

signals:
    void clicked(const QString &directoryPath);

protected:
    void mousePressEvent(QMouseEvent *event) override {
        emit clicked(directoryPath);
        QLabel::mousePressEvent(event);
    }

private:
    QString directoryPath;
};
