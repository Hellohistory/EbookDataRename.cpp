#include "tableviewmanager.h"
#include <QDir>
#include <QMessageBox>

TableViewManager::TableViewManager(QTableWidget *tableWidget)
    : QObject(), m_tableWidget(tableWidget), m_filePathMap() {

}

void TableViewManager::updateTableWidget(const QString &folderPath, bool updateRowCount) {
    QDir directory(folderPath);
    QStringList files = directory.entryList(QDir::Files);

    int startingRow = m_tableWidget->rowCount();
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

        emit progressUpdated(i + 1); // 更新进度
    }

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

            emit progressUpdated(row + 1); // 更新进度
        }
    }
}

void TableViewManager::checkNestedFolders(const QString &folderPath) {
    QDir directory(folderPath);
    QStringList subDirs = directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    foreach (const QString &subDir, subDirs) {
        QString subFolderPath = directory.absoluteFilePath(subDir);
        checkNestedFolders(subFolderPath); // 递归检查子文件夹
        updateTableWidget(subFolderPath, true); // 添加子文件夹中的文件，并更新行数
    }
}


const QMap<QString, QString>& TableViewManager::filePathMap() const {
    return m_filePathMap;
}
