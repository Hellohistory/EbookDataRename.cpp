#include "databasemanager.h"
#include <QDir>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

DatabaseManager::DatabaseManager(const QString &path) : folderPath(path) {
    initializeDatabaseConnections();
}

DatabaseManager::~DatabaseManager() {
    // 关闭所有数据库连接
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
}

void DatabaseManager::initializeDatabaseConnections() {
    QDir dbDir(folderPath);
    QStringList dbFiles = dbDir.entryList(QStringList() << "*.db", QDir::Files);

    foreach (const QString &dbFile, dbFiles) {
        QString fullPath = dbDir.absoluteFilePath(dbFile);
        if (!QSqlDatabase::contains(dbFile)) {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", dbFile);
            db.setDatabaseName(fullPath);
            if (!db.open()) {
                // 无法连接数据库
            }
        }
    }
}

QSqlDatabase DatabaseManager::getDatabaseConnection(const QString &connectionName) {
    if (QSqlDatabase::contains(connectionName)) {
        return QSqlDatabase::database(connectionName);
    } else {
        // 数据库连接未找到
        return QSqlDatabase();
    }
}

QString DatabaseManager::queryTitleFromDatabase(const QString &ssCode) {
    QDir dbDir(folderPath);
    QStringList dbFiles = dbDir.entryList(QStringList() << "*.db", QDir::Files);
    QString title;

    foreach (const QString &dbFile, dbFiles) {
        QSqlDatabase db = getDatabaseConnection(dbFile);
        if (db.isOpen() || db.open()) {
            QSqlQuery query(db);
            query.prepare("SELECT title FROM books WHERE SS_code = :ssCode");
            query.bindValue(":ssCode", ssCode);

            if (query.exec() && query.next()) {
                title = query.value(0).toString();
                // 在数据库文件找到标题
                break; // 找到结果后跳出循环
            } else {
                // 在数据库文件未找到匹配项
            }
        }
    }
    return title.isEmpty() ? "无此列" : title;
}
