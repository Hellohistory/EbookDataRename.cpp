#include "mainwindow.h"
#include <QHeaderView>
#include "worker.h"
#include <QMessageBox>

MainGUI::MainGUI(QWidget *parent) : QMainWindow(parent) {
    this->setWindowTitle("EbookDataRename");
    this->setMinimumSize(800, 600);

    QWidget *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    this->setupLayout(mainLayout); // 设置布局和UI组件

    modeManager = new ModeManager(localModeRadioButton, local_db_lineedit, selectFolderButton, remote_url_lineedit, this);
    modeManager->updateMode(); // 初始模式更新

    tableViewManager = new TableViewManager(tableWidget);
    connect(tableViewManager, &TableViewManager::progressUpdated, progressBar, &QProgressBar::setValue);

    connect(localModeRadioButton, &QRadioButton::toggled, this, &MainGUI::onModeChanged);

    dbManager = new DatabaseManager(""); // 初始化 DatabaseManager 实例
    connect(queryButton, &QPushButton::clicked, this, [=]() {
        tableViewManager->updateTitlesFromDatabase(dbManager);
    });

    connect(renameButton, &QPushButton::clicked, this, &MainGUI::initiateRenaming);
}


void MainGUI::selectTargetFolder() {
    QString selectedFolder = QFileDialog::getExistingDirectory(
        this,
        tr("选择目标文件夹"),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );

    if (!selectedFolder.isEmpty()) {
        targetFolderLineEdit->setText(selectedFolder);

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "检测到嵌套文件夹", "是否处理所有嵌套文件夹？",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            tableViewManager->checkNestedFolders(selectedFolder); // 只调用一次
        }

        tableViewManager->updateTableWidget(selectedFolder, true);
    }
}


void MainGUI::onModeChanged() {
    modeManager->updateMode();
}

void MainGUI::selectDatabase() {
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("选择数据库文件夹"));
    if (!folderPath.isEmpty()) {
        local_db_lineedit->setText(folderPath);

        delete dbManager; // 先清理旧的实例（如果有）
        dbManager = new DatabaseManager(folderPath);
    }
}

void MainGUI::initiateRenaming() {
    progressBar->setMaximum(tableWidget->rowCount());
    progressBar->setValue(0);

    const QMap<QString, QString>& filePathMap = tableViewManager->filePathMap();

    // 创建工作线程
    QThread *thread = new QThread;
    Worker *worker = new Worker(filePathMap);
    worker->moveToThread(thread);

    // 连接Worker的信号到MainGUI的槽
    connect(worker, &Worker::progressUpdated, this, [=](int progress){
        progressBar->setValue(progress);
    });
    connect(worker, &Worker::renameResult, this, [=](int row, bool success){
        QString status = success ? "已重命名" : "重命名失败";
                             if (row < tableWidget->rowCount()) {
            tableWidget->setItem(row, 2, new QTableWidgetItem(status));
        }
    });
    connect(worker, &Worker::finished, this, [=](){
        progressBar->setValue(tableWidget->rowCount());
        thread->quit();
        worker->deleteLater();
        thread->deleteLater();
    });
    connect(thread, &QThread::started, worker, &Worker::process);

    // 开始线程
    thread->start();
}



void MainGUI::setupLayout(QVBoxLayout *mainLayout) {
    QHBoxLayout *modeLayout = new QHBoxLayout();
    localModeRadioButton = new QRadioButton("本地模式");
    remoteModeRadioButton = new QRadioButton("远程模式");
    localModeRadioButton->setChecked(true);
    modeLayout->addWidget(localModeRadioButton);
    modeLayout->addWidget(remoteModeRadioButton);
    mainLayout->addLayout(modeLayout);

    targetFolderLineEdit = new QLineEdit();
    selectTargetFolderButton = new QPushButton("选择目标文件夹");
    QHBoxLayout *targetFolderLayout = new QHBoxLayout();
    targetFolderLayout->addWidget(targetFolderLineEdit);
    targetFolderLayout->addWidget(selectTargetFolderButton);
    mainLayout->addLayout(targetFolderLayout);

    connect(selectTargetFolderButton, &QPushButton::clicked, this, &MainGUI::selectTargetFolder);
    connect(localModeRadioButton, &QRadioButton::toggled, this, &MainGUI::onModeChanged);

    local_db_lineedit = new QLineEdit();
    selectFolderButton = new QPushButton("选择数据库文件夹");
    QHBoxLayout *folderLayout = new QHBoxLayout();
    folderLayout->addWidget(local_db_lineedit);
    folderLayout->addWidget(selectFolderButton);
    mainLayout->addLayout(folderLayout);

    connect(selectFolderButton, &QPushButton::clicked, this, &MainGUI::selectDatabase);

    remote_url_lineedit = new QLineEdit();
    remote_url_lineedit->setPlaceholderText("输入API地址");
    mainLayout->addWidget(remote_url_lineedit);

    changeExtensionCheckBox = new QCheckBox("将 .uvz 改为 .zip");
    traditionalSimplifiedCheckBox = new QCheckBox("呈现效果为繁体书名");
    QHBoxLayout *checkBoxLayout = new QHBoxLayout();
    checkBoxLayout->addWidget(changeExtensionCheckBox);
    checkBoxLayout->addWidget(traditionalSimplifiedCheckBox);
    mainLayout->addLayout(checkBoxLayout);

    tableWidget = new QTableWidget();
    tableWidget->setColumnCount(3); // 设置列数
    tableWidget->setHorizontalHeaderLabels({"文件名", "重命名后文件名", "状态"});

    // 设置列宽自适应
    QHeaderView* headerView = tableWidget->horizontalHeader();
    headerView->setSectionResizeMode(0, QHeaderView::Stretch);
    headerView->setSectionResizeMode(1, QHeaderView::Stretch);
    headerView->setSectionResizeMode(2, QHeaderView::Fixed);
    headerView->resizeSection(2, 100);

    mainLayout->addWidget(tableWidget);

    queryButton = new QPushButton("查询文件名");
    renameButton = new QPushButton("开始重命名");
    mainLayout->addWidget(queryButton);
    mainLayout->addWidget(renameButton);

    connect(renameButton, &QPushButton::clicked, this, &MainGUI::initiateRenaming);

    progressBar = new QProgressBar();
    mainLayout->addWidget(progressBar);
}

MainGUI::~MainGUI() {
    // 如果线程正在运行，则等待它完成
    if (workerThread.isRunning()) {
        workerThread.quit();
        workerThread.wait();
    }

    delete dbManager; // 清理 DatabaseManager 实例
    delete tableViewManager; // 清理 TableViewManager 实例
    delete modeManager; // 清理 ModeManager 实例
}

