#include <QApplication>
#include <QFile>
#include <QTime>
#include "katlascontrol.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	KAtlasControl gpcontrol(0);
	gpcontrol.show();
	for ( int i = 1; i < argc; ++i ) {
		if ( QFile::exists( app.arguments().at( i ) ) == true )
			gpcontrol.addPlaceMarkFile( argv[i] );

		if ( strcmp( argv[ i ], "--timedemo" ) == 0 ) {
			qDebug( "Running timedemo, stand by..." );
			QTime t;
			t.start();

			for ( int j = 0; j < 100; ++j ) {
				gpcontrol.moveLeft();
//				gpcontrol.moveUp();
			}
/*
			for ( int j = 0; j < 10; ++j ) {

				for ( int i = 0; i < 5; ++i ){
					gpcontrol.moveLeft();
				}
				for ( int i = 0; i < 5; ++i ){
					gpcontrol.moveRight();
				}
			}
*/
			qDebug( "Timedemo finished in %ims", t.elapsed() );
			qDebug() <<  QString("= %1 fps").arg(100000.0/(float)(t.elapsed()));
			return 0;
		}			 
	}
	return app.exec();
}
