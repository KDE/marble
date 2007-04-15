#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
 
#include "KdeMainWindow.h"
 
int main (int argc, char *argv[])
{
    KAboutData aboutData( "marble", 
        "Marble WorldAtlas", "0.3.5", "A generic geographical",
        KAboutData::License_LGPL, "(c) 2007" );
    KCmdLineArgs::init( argc, argv, &aboutData );
  
    KApplication app;
 
    MainWindow* window = new MainWindow();
    window->show();
 
    return app.exec();
}