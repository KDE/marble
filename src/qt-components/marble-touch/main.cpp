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
#include "MarbleDebug.h"
#include "global.h"

using namespace Marble;

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );
    app.setProperty( "NoMStyle", true );

    //MarbleGlobal::Profiles profiles = MarbleGlobal::detectProfiles();
    /** @todo FIXME: Which compiler flag to use in detectProfiles for harmattan? */
    MarbleGlobal::Profiles profiles = MarbleGlobal::SmallScreen | MarbleGlobal::HighResolution;

    QStringList args = QApplication::arguments();
    if ( args.contains( "-h" ) || args.contains( "--help" ) ) {
        qWarning() << "Usage: marble-touch [options]";
        qWarning();
        qWarning() << "general options:";
        qWarning() << "  --portrait ................. Force a rotation by 90 degree";
        qWarning() << "  --debug-info ............... write (more) debugging information to the console";
        qWarning();
        qWarning() << "profile options (note that marble should automatically detect which profile to use. Override that with the options below):";
        qWarning() << "  --smallscreen .............. Enforce the profile for devices with small screens (e.g. smartphones)";
        qWarning() << "  --highresolution ........... Enforce the profile for devices with high resolution (e.g. desktop computers)";
        qWarning() << "  --nosmallscreen ............ Deactivate the profile for devices with small screens (e.g. smartphones)";
        qWarning() << "  --nohighresolution ......... Deactivate the profile for devices with high resolution (e.g. desktop computers)";

        return 0;
    }

    bool portraitMode = false;
    for ( int i = 1; i < args.count(); ++i ) {
        QString const arg = args.at( i );

        if ( arg == "--debug-info" ) {
            MarbleDebug::enable = true;
        }
        else if ( arg == "--portrait" ) {
            portraitMode = true;
        }
        else if ( arg == "--smallscreen" ) {
            profiles |= MarbleGlobal::SmallScreen;
        }
        else if ( arg == "--nosmallscreen" ) {
            profiles &= ~MarbleGlobal::SmallScreen;
        }
        else if ( arg == "--highresolution" ) {
            profiles |= MarbleGlobal::HighResolution;
        }
        else if ( arg == "--nohighresolution" ) {
            profiles &= ~MarbleGlobal::HighResolution;
        }
    }

    MarbleGlobal::getInstance()->setProfiles( profiles );

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
    if ( portraitMode ) {
        view.resize( view.initialSize().height(), view.initialSize().width() );
        view.setTransform( QTransform().rotate( 90 ) );
    } else {
        view.resize( view.initialSize().width(), view.initialSize().height() );
    }
    view.show();
#endif

    return app.exec();
}
