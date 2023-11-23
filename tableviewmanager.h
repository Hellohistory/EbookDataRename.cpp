#ifndef TABLEVIEWMANAGER_H
#define TABLEVIEWMANAGER_H

#include <QTableWidget>
#include <QString>
#include <QMap>
#include <QDir>
#include "databasemanager.h"

class TableViewManager {
public:
    explicit TableViewManager(QTableWidget *tableWidget);
    void updateTableWidget(const QString &folderPath, bool updateRowCount);
    void updateTitlesFromDatabase(DatabaseManager *dbManager);
    void checkNestedFolders(const QString &folderPath);

    // 添加一个公共方法来获取文件路径映射
    const QMap<QString, QString>& filePathMap() const;

private:
    QTableWidget *m_tableWidget;
    QMap<QString, QString> m_filePathMap; // 文件名到文件路径的映射
};

#endif // TABLEVIEWMANAGER_H
