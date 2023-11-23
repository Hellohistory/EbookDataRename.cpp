// worker.h
#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QMap>

class Worker : public QObject {
    Q_OBJECT

public:
    explicit Worker(const QMap<QString, QString>& filePathMap, QObject *parent = nullptr);

signals:
    void renameResult(int row, bool success);
    void progressUpdated(int value);
    void finished();

public slots:
    void process();

private:
    QMap<QString, QString> filePathMap;
};

#endif // WORKER_H
