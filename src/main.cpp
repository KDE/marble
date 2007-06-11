//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include <QtGui/QApplication>
#include <QtCore/QFile>
#include <QtCore/QTime>

#include "katlascontrol.h"


#if STATIC_BUILD
 #include <QtCore/QtPlugin>
 Q_IMPORT_PLUGIN(qjpeg)
 Q_IMPORT_PLUGIN(qsvg)
#endif


int main(int argc, char *argv[])
{
    QApplication   app( argc, argv );
    KAtlasControl  katlascontrol( 0 );

    katlascontrol.show();
    // katlascontrol.addPlaceMarkFile( "../data/placemarks/stdplacemarks.kml" );
    // Parse all arguments
    for ( int i = 1; i < argc; ++i ) {
	if ( strcmp( argv[ i ], "--timedemo" ) == 0 ) {
	    qDebug( "Running timedemo, stand by..." );
	    QTime t;
	    t.start();

	    for ( int j = 0; j < 100; ++j ) {
		for ( int k = 0; k < 10; ++k ) {
		    katlascontrol.moveRight();
		}
		for ( int k = 0; k < 10; ++k ) {
		    katlascontrol.moveLeft();
		}
		// katlascontrol.moveUp();
	    }
#if 0
            for ( int j = 0; j < 10; ++j ) {

                for ( int i = 0; i < 5; ++i ){
                    katlascontrol.moveLeft();
                }
                for ( int i = 0; i < 5; ++i ){
                    katlascontrol.moveRight();
                }
            }
#endif
	    qDebug( "Timedemo finished in %ims", t.elapsed() );
	    qDebug() <<  QString("= %1 fps").arg(2000*1000/(double)(t.elapsed()));
	    return 0;
	}
	else if ( QFile::exists( app.arguments().at( i ) ) )
	    katlascontrol.addPlaceMarkFile( argv[i] );
    }

    return app.exec();
}
