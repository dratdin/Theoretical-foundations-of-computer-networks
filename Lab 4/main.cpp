#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.configurateGUI();
    w.connectPorts("ttys003","ttys004");
    w.show();
    return a.exec();
}
