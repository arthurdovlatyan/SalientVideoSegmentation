#include "MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QGuiApplication::setApplicationDisplayName("Salient Video Object Detection");
    MainWindow w;
    w.initialize();
    w.show();
    return a.exec();
}
