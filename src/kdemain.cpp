#include <KApplication>
#include <KLocale>
#include <KAboutData>
#include <KCmdLineArgs>
 
#include "KdeMainWindow.h"

#if STATIC_BUILD
 #include <QtCore/QtPlugin>
 Q_IMPORT_PLUGIN(qjpeg)
 Q_IMPORT_PLUGIN(qsvg)
#endif
 
int main (int argc, char *argv[])
{
    KAboutData aboutData( "marble", 
        "Marble Desktop Globe", "0.3.5", I18N_NOOP("A World Atlas."),
        KAboutData::License_LGPL, I18N_NOOP("(c) 2007") );
    aboutData.addAuthor("Torsten Rahn", "Original author and maintainer", "rahn@kde.org");
    aboutData.addAuthor("Inge Wallin", "co-maintainer", "inge@lysator.liu.se");
    aboutData.setHomepage("http://edu.kde.org/marble");

    KCmdLineArgs::init( argc, argv, &aboutData );

    KApplication app;

    MainWindow *window = new MainWindow();
    window->resize(680, 640);
    window->show();

    return app.exec();
}
