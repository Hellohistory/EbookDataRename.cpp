#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QProgressBar>
#include <QRadioButton>
#include <QCheckBox>
#include <QFileDialog>
#include <QTableWidgetItem>
#include <QThread> // 引入 QThread 类
#include "mode_manager.h"
#include "tableviewmanager.h"
#include "databasemanager.h"

class MainGUI : public QMainWindow {
    Q_OBJECT

public:
    MainGUI(QWidget *parent = nullptr);
    ~MainGUI(); // 析构函数

private slots:
    void selectTargetFolder();
    void onModeChanged();
    void selectDatabase();
    void initiateRenaming();

private:
    void setupLayout(QVBoxLayout *mainLayout);

    // UI组件
    QLineEdit *targetFolderLineEdit, *local_db_lineedit, *remote_url_lineedit;
    QPushButton *selectTargetFolderButton, *selectFolderButton, *queryButton, *renameButton;
    QRadioButton *localModeRadioButton, *remoteModeRadioButton;
    QCheckBox *changeExtensionCheckBox, *traditionalSimplifiedCheckBox;
    QTableWidget *tableWidget;
    QProgressBar *progressBar;

    // 成员变量
    ModeManager *modeManager; // ModeManager 实例
    TableViewManager *tableViewManager; // TableViewManager 实例
    DatabaseManager *dbManager; // DatabaseManager 实例
    QThread workerThread; // 用于文件重命名操作的线程
};

#endif // MAINWINDOW_H
