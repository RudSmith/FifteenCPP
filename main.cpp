#include "mainwindow.h"

#include <QApplication>

// Стандартный main в qt
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
