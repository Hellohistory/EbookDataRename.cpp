#ifndef TABLEVIEWMANAGER_H
#define TABLEVIEWMANAGER_H

#include <QTableWidget>
#include <QString>
#include <QMap>
#include <QDir>
#include "databasemanager.h"

class TableViewManager : public QObject {
    Q_OBJECT // 必须添加这个宏来支持信号和槽

public:
    explicit TableViewManager(QTableWidget *tableWidget);
    void updateTableWidget(const QString &folderPath, bool updateRowCount);
    void updateTitlesFromDatabase(DatabaseManager *dbManager);
    void checkNestedFolders(const QString &folderPath);
    const QMap<QString, QString>& filePathMap() const;

signals:
    void progressUpdated(int value); // 添加进度更新信号

private:
    QTableWidget *m_tableWidget;
    QMap<QString, QString> m_filePathMap; // 文件名到文件路径的映射
};

#endif // TABLEVIEWMANAGER_H
