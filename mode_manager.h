#ifndef MODE_MANAGER_H
#define MODE_MANAGER_H

#include <QObject>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>

class ModeManager : public QObject {
    Q_OBJECT

public:
    ModeManager(QRadioButton *localModeRadioButton, QLineEdit *localDbLineEdit,
                QPushButton *selectFolderButton, QLineEdit *remoteUrlLineEdit, QObject *parent = nullptr);

    void updateMode();

private:
    QRadioButton *localModeRadioButton;
    QLineEdit *localDbLineEdit;
    QPushButton *selectFolderButton;
    QLineEdit *remoteUrlLineEdit;
};

#endif // MODE_MANAGER_H
