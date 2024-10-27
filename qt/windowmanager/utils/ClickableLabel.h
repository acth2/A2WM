#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
#include <QPalette>
#include <QGraphicsOpacityEffect>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(const QString &text, const QString &dirPath, QWidget *parent = nullptr)
        : QLabel(text, parent), directoryPath(dirPath) {
        opacityEffect = new QGraphicsOpacityEffect(this);
        setGraphicsEffect(opacityEffect);
        opacityEffect->setOpacity(1.0);

        setAttribute(Qt::WA_TranslucentBackground);
        setMinimumSize(64, 64);
    }

    QString getDirectoryPath() const { return directoryPath; }

signals:
    void clicked(const QString &directoryPath);

protected:
    void mousePressEvent(QMouseEvent *event) override {
        QPalette palette = this->palette();
        palette.setColor(QPalette::Window, Qt::black);
        setPalette(palette);
        update();

        opacityEffect->setOpacity(0.6);

        emit clicked(directoryPath);
        QLabel::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        QPalette palette = this->palette();
        palette.setColor(QPalette::Window, Qt::transparent);
        setPalette(palette);
        update();

        opacityEffect->setOpacity(1.0);
        QLabel::mouseReleaseEvent(event);
    }

private:
    QString directoryPath;
    QGraphicsOpacityEffect *opacityEffect;
};
