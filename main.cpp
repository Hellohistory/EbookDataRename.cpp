#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainGUI mainWindow;
    mainWindow.show();
    return app.exec();
}
