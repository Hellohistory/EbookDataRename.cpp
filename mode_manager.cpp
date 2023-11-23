#include "mode_manager.h"

ModeManager::ModeManager(QRadioButton *localModeRadioButton, QLineEdit *localDbLineEdit,
                         QPushButton *selectFolderButton, QLineEdit *remoteUrlLineEdit, QObject *parent)
    : QObject(parent), localModeRadioButton(localModeRadioButton),
    localDbLineEdit(localDbLineEdit), selectFolderButton(selectFolderButton),
    remoteUrlLineEdit(remoteUrlLineEdit) {}

void ModeManager::updateMode() {
    bool isLocalMode = localModeRadioButton->isChecked();

    // 显示或隐藏本地模式相关的输入框和按钮
    localDbLineEdit->setVisible(isLocalMode);
    selectFolderButton->setVisible(isLocalMode);

    // 显示或隐藏远程模式相关的输入框
    remoteUrlLineEdit->setVisible(!isLocalMode);
}
