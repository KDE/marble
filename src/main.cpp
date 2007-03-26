#include <QtGui/QApplication>
#include <QtCore/QFile>
#include <QtCore/QTime>
#include "katlascontrol.h"

#if STATIC_BUILD
 Q_IMPORT_PLUGIN(qjpeg)
 Q_IMPORT_PLUGIN(qsvg)
#endif

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	KAtlasControl katlascontrol(0);
	katlascontrol.show();
// katlascontrol.addPlaceMarkFile( "../data/placemarks/stdplacemarks.kml" );
	for ( int i = 1; i < argc; ++i ) {
		if ( QFile::exists( app.arguments().at( i ) ) == true )
			katlascontrol.addPlaceMarkFile( argv[i] );

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
//				katlascontrol.moveUp();
			}
/*
			for ( int j = 0; j < 10; ++j ) {

				for ( int i = 0; i < 5; ++i ){
					katlascontrol.moveLeft();
				}
				for ( int i = 0; i < 5; ++i ){
					katlascontrol.moveRight();
				}
			}
*/
			qDebug( "Timedemo finished in %ims", t.elapsed() );
			qDebug() <<  QString("= %1 fps").arg(2000*1000/(float)(t.elapsed()));
			return 0;
		}			 
	}
	return app.exec();
}
