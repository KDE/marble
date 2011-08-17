// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

// A QML-interface of Marble for the Meego operating system.

#include <QtGui/QApplication>
#include <QtDeclarative/QtDeclarative>

#include "MainWindow.h"

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );
    app.setProperty( "NoMStyle", true );

    QDir::setCurrent( app.applicationDirPath() );

    // Create main window based on QML.
    MainWindow window;
    window.setSource( QUrl( "qrc:/main.qml" ) );

#ifdef __arm__
    // Window takes up full screen on arm (mobile) devices.
    window.showFullScreen();
#else
    window.resize( window.initialSize().width(), window.initialSize().height() );
    window.show();
#endif

    return app.exec();
}
