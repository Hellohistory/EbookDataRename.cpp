#include "mainwindow.h"
#include <QHeaderView>

MainGUI::MainGUI(QWidget *parent) : QMainWindow(parent) {
    this->setWindowTitle("EbookDataRename");
    this->setMinimumSize(800, 600);

    QWidget *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    this->setupLayout(mainLayout); // 首先设置布局和UI组件

    // 然后创建 ModeManager 实例
    modeManager = new ModeManager(localModeRadioButton, local_db_lineedit, selectFolderButton, remote_url_lineedit, this);

    // 初始模式更新
    modeManager->updateMode();

    // 连接信号和槽
    connect(localModeRadioButton, &QRadioButton::toggled, this, &MainGUI::onModeChanged);

    tableViewManager = new TableViewManager(tableWidget);

    // 初始化 DatabaseManager 实例，这里假设先以空字符串初始化
    dbManager = new DatabaseManager("");

    connect(queryButton, &QPushButton::clicked, this, [=]() {
        tableViewManager->updateTitlesFromDatabase(dbManager);
    });
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

        // 首先检查并处理嵌套文件夹
        tableViewManager->checkNestedFolders(selectedFolder);

        // 更新主文件夹的文件到表格，此时应该更新行数
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
    // 获取 filePathMap 的引用
    const QMap<QString, QString>& filePathMap = tableViewManager->filePathMap();

    // 遍历表格的所有行
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        // 获取原始文件名和新文件名
        QTableWidgetItem *originalItem = tableWidget->item(row, 0);
        QTableWidgetItem *newNameItem = tableWidget->item(row, 1);

        if (originalItem && newNameItem) {
            QString originalFileName = originalItem->text();
            QString newFileName = newNameItem->text();

            // 从 filePathMap 中获取原始文件的完整路径
            QString originalFilePath = filePathMap.value(originalFileName);

            // 检查是否需要执行重命名
            if (!newFileName.isEmpty() && newFileName != "无此列" && newFileName != "无效的 SS_code" && newFileName != "已找到记录") {
                // 构建新文件的完整路径
                QFileInfo fileInfo(originalFilePath);
                QString newFilePath = fileInfo.absoluteDir().absoluteFilePath(newFileName);

                // 执行重命名
                if (QFile::rename(originalFilePath, newFilePath)) {
                    // 更新状态列为“已重命名”
                    tableWidget->setItem(row, 2, new QTableWidgetItem("已重命名"));
                } else {
                    // 更新状态列为“重命名失败”
                    tableWidget->setItem(row, 2, new QTableWidgetItem("重命名失败"));
                }
            }
        }
    }
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
    delete dbManager; // 清理 DatabaseManager 实例
    delete tableViewManager; // 清理 TableViewManager 实例
    delete modeManager; // 清理 ModeManager 实例
}
