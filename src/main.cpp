#include "ppgViewer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ppgViewer window;
    window.show();
    window.initialize();
    return a.exec();
}
