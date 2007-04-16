#include <QtGui/QApplication>

#include "QtMainWindow.h"

#if STATIC_BUILD
 #include <QtCore/QtPlugin>
 Q_IMPORT_PLUGIN(qjpeg)
 Q_IMPORT_PLUGIN(qsvg)
#endif

int main(int argc, char *argv[])
{
//    Q_INIT_RESOURCE(application);

    QApplication app(argc, argv);

    MainWindow *window = new MainWindow();
    window->resize(680, 640);
    window->show();

    return app.exec();
}
