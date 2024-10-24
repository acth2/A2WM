#include <QApplication>
#include <QMainWindow>
#include <QTreeView>
#include <QFileSystemModel>
#include <QSplitter>
#include <QListView>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>

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

        QPushButton* homeButton = new QPushButton("Home", this);
        connect(homeButton, &QPushButton::clicked, [=]() {
            QString homePath = QDir::homePath();
            listView->setRootIndex(fileModel->setRootPath(homePath));
            treeView->setRootIndex(dirModel->index(homePath));
        });

        QSplitter* splitter = new QSplitter(this);
        splitter->addWidget(treeView);
        splitter->addWidget(listView);
        splitter->setStretchFactor(1, 1);

        QVBoxLayout* layout = new QVBoxLayout();
        layout->addWidget(homeButton);
        layout->addWidget(splitter);
        QWidget* centralWidget = new QWidget(this);
        centralWidget->setLayout(layout);
        setCentralWidget(centralWidget);

        listView->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(listView, &QListView::customContextMenuRequested, [=](const QPoint& pos) {
            QModelIndex index = listView->indexAt(pos);
            if (index.isValid()) {
                QMenu menu(this);
                QAction* deleteAction = menu.addAction("Delete");
                QAction* createFileAction = menu.addAction("Create New File");

                connect(deleteAction, &QAction::triggered, [=]() {
                    QString filePath = fileModel->fileInfo(index).absoluteFilePath();
                    if (QFile::remove(filePath)) {
                        QMessageBox::information(this, "Success", "File deleted.");
                    } else {
                        QMessageBox::warning(this, "Error", "Failed to delete file.");
                    }
                });

                connect(createFileAction, &QAction::triggered, [=]() {
                    bool ok;
                    QString fileName = QInputDialog::getText(this, "Create New File", "File name:", QLineEdit::Normal, "", &ok);
                    if (ok && !fileName.isEmpty()) {
                        QString dirPath = fileModel->fileInfo(index).absolutePath();
                        QString newFilePath = QDir(dirPath).filePath(fileName);
                        QFile newFile(newFilePath);
                        if (newFile.open(QIODevice::ReadWrite)) {
                            newFile.close();
                            QMessageBox::information(this, "Success", "File created.");
                        } else {
                            QMessageBox::warning(this, "Error", "Failed to create file.");
                        }
                    }
                });

                menu.exec(QCursor::pos());
            }
        });

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
