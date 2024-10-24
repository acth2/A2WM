#include <QApplication>
#include <QMainWindow>
#include <QTreeView>
#include <QFileSystemModel>
#include <QSplitter>
#include <QListView>
#include <QVBoxLayout>
#include <QHeaderView>

class FileExplorer : public QMainWindow {
    Q_OBJECT

public:
    FileExplorer(QWidget* parent = nullptr) : QMainWindow(parent) {
        QFileSystemModel* dirModel = new QFileSystemModel(this);
        dirModel->setRootPath(QDir::rootPath());
        dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);

        QFileSystemModel* fileModel = new QFileSystemModel(this);
        fileModel->setRootPath(QDir::rootPath());
        fileModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);

        QTreeView* treeView = new QTreeView(this);
        treeView->setModel(dirModel);
        treeView->setRootIndex(dirModel->index(QDir::rootPath()));
        treeView->setHeaderHidden(true);
        treeView->header()->setStretchLastSection(false);
        treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

        QListView* listView = new QListView(this);
        listView->setModel(fileModel);

        connect(treeView->selectionModel(), &QItemSelectionModel::currentChanged,
                [=](const QModelIndex& current, const QModelIndex&) {
                    QString path = dirModel->fileInfo(current).absoluteFilePath();
                    listView->setRootIndex(fileModel->setRootPath(path));
                });

        QSplitter* splitter = new QSplitter(this);
        splitter->addWidget(treeView);
        splitter->addWidget(listView);
        splitter->setStretchFactor(1, 1);

        setCentralWidget(splitter);
        setWindowTitle("Qt File Explorer");
        resize(800, 600);
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    FileExplorer explorer;
    explorer.show();
    return app.exec();
}

#include "fileexplorer.moc"
