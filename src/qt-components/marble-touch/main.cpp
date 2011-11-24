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

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );
    app.setProperty( "NoMStyle", true );

    QDir::setCurrent( app.applicationDirPath() );

    // Create main window based on QML.
    QDeclarativeView view;
    view.setSource( QUrl( "qrc:/main.qml" ) );
    view.setAttribute( Qt::WA_OpaquePaintEvent );
    view.setAttribute( Qt::WA_NoSystemBackground );
    view.viewport()->setAttribute( Qt::WA_OpaquePaintEvent );
    view.viewport()->setAttribute( Qt::WA_NoSystemBackground );

#ifdef __arm__
    // Window takes up full screen on arm (mobile) devices.
    view.showFullScreen();
#else
    if ( app.arguments().contains( "--portrait" ) ) {
        view.resize( view.initialSize().height(), view.initialSize().width() );
        view.setTransform( QTransform().rotate( 90 ) );
    } else {
        view.resize( view.initialSize().width(), view.initialSize().height() );
    }
    view.show();
#endif

    return app.exec();
}
