#include "tableviewmanager.h"
#include <QDir>
#include <QMessageBox>

TableViewManager::TableViewManager(QTableWidget *tableWidget)
    : m_tableWidget(tableWidget), m_filePathMap() {

}

void TableViewManager::updateTableWidget(const QString &folderPath, bool updateRowCount) {
    QDir directory(folderPath);
    QStringList files = directory.entryList(QDir::Files);

    // 正确计算开始的行号
    int startingRow = m_tableWidget->rowCount();

    // 如果需要更新行数，设置新的行数为当前行数加上新文件的数量
    if (updateRowCount) {
        m_tableWidget->setRowCount(startingRow + files.size());
    }

    for (int i = 0; i < files.size(); ++i) {
        int currentRow = startingRow + i;
        QString fullPath = directory.absoluteFilePath(files.at(i));
        QString fileName = QFileInfo(fullPath).fileName();
        m_filePathMap[fileName] = fullPath;

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(fileName);
        QTableWidgetItem *titleItem = new QTableWidgetItem(fileName);
        QTableWidgetItem *statusItem = new QTableWidgetItem("未查询");

        m_tableWidget->setItem(currentRow, 0, fileNameItem);
        m_tableWidget->setItem(currentRow, 1, titleItem);
        m_tableWidget->setItem(currentRow, 2, statusItem);
    }

    // 确保在此方法结束时调用刷新，如果在这里添加了新的行
    m_tableWidget->viewport()->update();
}

void TableViewManager::updateTitlesFromDatabase(DatabaseManager *dbManager) {
    for (int row = 0; row < m_tableWidget->rowCount(); ++row) {
        QTableWidgetItem *fileNameItem = m_tableWidget->item(row, 0);
        if (fileNameItem) {
            QString originalFileName = fileNameItem->text();
            QString ssCode = originalFileName.section('.', 0, 0);

            QString title = dbManager->queryTitleFromDatabase(ssCode);
            if (title != "无此列") {
                QString extension = originalFileName.section('.', -1);
                m_tableWidget->item(row, 1)->setText(title + "." + extension);
                m_tableWidget->item(row, 2)->setText("已查询");
            } else {
                m_tableWidget->item(row, 1)->setText(originalFileName);
                m_tableWidget->item(row, 2)->setText("无此内容");
            }
        }
    }
}

void TableViewManager::checkNestedFolders(const QString &folderPath) {
    QDir directory(folderPath);
    QStringList subDirs = directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    if (!subDirs.isEmpty()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(m_tableWidget, "检测到嵌套文件夹", "是否处理嵌套文件夹？",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            foreach (const QString &subDir, subDirs) {
                QString subFolderPath = directory.absoluteFilePath(subDir);
                checkNestedFolders(subFolderPath);  // 递归检查子文件夹
                updateTableWidget(subFolderPath, true); // 添加子文件夹中的文件，并更新行数
            }
        }
    }
}

const QMap<QString, QString>& TableViewManager::filePathMap() const {
    return m_filePathMap;
}
