#include "mainwindow.h"
#include "client.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow load;
    load.show();

    return a.exec();
}
