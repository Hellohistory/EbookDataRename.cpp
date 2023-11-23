#include "worker.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>

Worker::Worker(const QMap<QString, QString>& filePathMap, QObject *parent)
    : QObject(parent), filePathMap(filePathMap) {}

void Worker::process() {
    int row = 0;

    for (auto it = filePathMap.constBegin(); it != filePathMap.constEnd(); ++it, ++row) {
        QString originalFileName = it.key();
        QString newFileName = it.value();

        if (!newFileName.isEmpty() && newFileName != "无此列" && newFileName != "无效的 SS_code" && newFileName != "已找到记录") {
            QFileInfo fileInfo(originalFileName);
            QString newFilePath = fileInfo.absoluteDir().absoluteFilePath(newFileName);

            bool renameSuccess = QFile::rename(originalFileName, newFilePath);
            emit renameResult(row, renameSuccess);
        } else {
            // 如果文件名不符合重命名条件，仍然需要发射结果以更新进度，但标记为失败
            emit renameResult(row, false);
        }

        // 发射进度更新信号
        emit progressUpdated(row + 1);
    }

    emit finished();
}
