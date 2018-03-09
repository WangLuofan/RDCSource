#include "RDCMainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RDCMainWindow w;
    w.show();

    return a.exec();
}
