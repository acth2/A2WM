#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

class SettingsApp : public QWidget {
    Q_OBJECT

public:
    explicit SettingsApp(QWidget *parent = nullptr);

private:
    QPushButton *customizationButton;
    QPushButton *systemInfoButton;
    QPushButton *systemSettingsButton;
    void setupUI();
};
