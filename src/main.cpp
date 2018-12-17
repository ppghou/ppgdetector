#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>

#include <VLCQtCore/Common.h>

#include "ppgViewer.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("ppgViewer");
    QCoreApplication::setAttribute(Qt::AA_X11InitThreads);

    QApplication app(argc, argv);
    VlcCommon::setPluginPath(app.applicationDirPath() + "/plugins");

    ppgViewer mainWindow;
    mainWindow.show();

    return app.exec();
}
