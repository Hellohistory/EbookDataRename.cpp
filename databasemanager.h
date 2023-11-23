#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>

class DatabaseManager {
public:
    explicit DatabaseManager(const QString &path);
    ~DatabaseManager();

    QString queryTitleFromDatabase(const QString &ssCode);

private:
    void initializeDatabaseConnections();
    QSqlDatabase getDatabaseConnection(const QString &connectionName);
    QString folderPath;
};

#endif // DATABASEMANAGER_H
