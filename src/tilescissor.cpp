#include "tilescissor.h"

#include <QtGui/QApplication>
#include <QtCore/QDebug>
#include <QtGui/QImage>
#include <QtGui/QImageReader>
#include <QtGui/QPainter>
#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtCore/QVector>

#include "katlasdirs.h"
#include "TileLoader.h"

TileScissor::TileScissor(const QString& prefix, const QString& installmap, const QString& dem):m_prefix(prefix), m_installmap(installmap),m_dem(dem){
	/* NOOP */
}

void TileScissor::createTiles() {

	QApplication::processEvents(); 

	QString srcpath = KAtlasDirs::path( "maps/earth/" + m_prefix + "/" + m_installmap );
	QString destpath = KAtlasDirs::localDir() + "/maps/earth/" + m_prefix + "/";

	qDebug() << "Creating tiles from: " << srcpath;
	QImageReader testimg( srcpath );

	QVector<QRgb> legpal;
		for ( int count = 0; count <= 255; count++){
		legpal.insert(count, qRgb(count, count, count));
	}

	int imgw = testimg.size().width();
	int imgh = testimg.size().height();
/*
	if ( imgw > 10800 || imgh > 10800 ){
		qDebug("Install map too large!");
		exit(-1);
	} 
*/	
	bool smooth = ( (imgw%675)%2 > 0 || (imgh%675)%2 > 0 ) ? true : false;

	int maxtilelevel = -1;
	int stdimgw = 0;
	int stdimgh = 0;

	int count = 0;
	while ( stdimgw < imgw ){
		maxtilelevel = count;
		stdimgw = 2 * 675 * TileLoader::levelToRow( maxtilelevel );
		count++;
	}
	stdimgh = stdimgw / 2;
	qDebug() << "Maximum tile level: " << maxtilelevel;


	if ( QDir( KAtlasDirs::localDir() + "/maps/earth/" + m_prefix ).exists() == false ) 
		( QDir::root() ).mkpath( KAtlasDirs::localDir() + "/maps/earth/" + m_prefix );

	int tilelevel = 0;

	QSize stdsize = QSize( 675,675 );

	// Counting total amount of tiles to be generated for progressbar
	int maxcount = 0;

	while ( tilelevel <= maxtilelevel ) {
		int mmaxit = TileLoader::levelToColumn( tilelevel );
		for ( int m=0; m < mmaxit; m++) { 
			int nmaxit = TileLoader::levelToRow( tilelevel );
			for ( int n=0; n < nmaxit; n++)
				maxcount++;
		}
		tilelevel++;	
	}

	qDebug() << maxcount << " tiles to be created in total.";

	int mmax = TileLoader::levelToColumn( maxtilelevel );
	int nmax = TileLoader::levelToRow( maxtilelevel );

	// Loading each row at highest spatial resolution and croping tiles
	int completed = 0;
	count = 0; // reset, so that it counts the percentage correctly
	QString tilename;

	// Creating directory structure for the highest level

	QString dirname( KAtlasDirs::localDir() + QString("/maps/earth/%1/%2").arg(m_prefix).arg(maxtilelevel) );
	if ( QDir( dirname ).exists() == false ) 
		( QDir::root() ).mkpath( dirname );
	for ( int n=0; n < nmax; n++) {
		QString dirname( KAtlasDirs::localDir() + QString("/maps/earth/%1/%2/%3").arg(m_prefix).arg(maxtilelevel).arg( n, 4, 10, QChar('0') ) );
		if ( QDir( dirname ).exists() == false ) 
			( QDir::root() ).mkpath( dirname );
	}

	for ( int n=0; n < nmax; n++){
		QApplication::processEvents(); 

		QRect rowsrc( 0, (int)((float)(n*imgh)/(float)(nmax)), imgw, 	(int)((float)(imgh)/(float)(nmax)) );

		QImage img( srcpath );

		QImage row = img.copy( rowsrc );
		QSize destsize( stdimgw, 675 );

		if ( smooth == true ){
			row = img.scaled(destsize,  Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		}

		if ( row.isNull() ) qDebug() << "Read-Error! Null QImage!";

		for ( int m=0; m < mmax; m++) {
			QApplication::processEvents(); 

			QImage tile = row.copy( m*imgw/mmax, 0, 675, 675 );

			tilename = destpath + QString("%1/%2/%2_%3.jpg").arg( maxtilelevel ).arg( n, 4, 10, QChar('0') ).arg( m, 4, 10, QChar('0') );

			if ( m_dem == "true" ){
				tile=tile.convertToFormat(QImage::Format_Indexed8, legpal, Qt::ThresholdDither);
			}

			bool noerr = tile.save( tilename, "jpg", 100);
			if ( noerr == false ) qDebug() << "Error while writing Tile: " << tilename;

			completed = (int) ( 90 * (float)(count) / (float)(maxcount) );	
			count++;
						
			emit progress( completed );
		}
	}

	qDebug() << "Tilelevel: " << maxtilelevel << " successfully created.";

	tilelevel = maxtilelevel - 1;

	// Now that we have the tiles at the highest resolution lets build them together four by four

	while( tilelevel >= 0 ) {

		int nmaxit =  TileLoader::levelToRow( tilelevel );;
		for ( int n=0; n < nmaxit; n++) {
			QString dirname( KAtlasDirs::localDir() + QString("/maps/earth/%1/%2/%3").arg(m_prefix).arg(tilelevel).arg( n, 4, 10, QChar('0') ) );
//			qDebug() << "dirname: " << dirname;
			if ( QDir( dirname ).exists() == false ) 
				( QDir::root() ).mkpath( dirname );

			int mmaxit = TileLoader::levelToColumn( tilelevel );;
			for ( int m=0; m < mmaxit; m++){

				tilename = destpath + QString("%1/%2/%2_%3.jpg").arg( tilelevel + 1 ).arg( 2*n, 4, 10, QChar('0') ).arg( 2*m, 4, 10, QChar('0') );
				QImage img_topleft( tilename );
				
				tilename = destpath + QString("%1/%2/%2_%3.jpg").arg( tilelevel + 1 ).arg( 2*n, 4, 10, QChar('0') ).arg( 2*m+1, 4, 10, QChar('0') );
				QImage img_topright( tilename );

				tilename = destpath + QString("%1/%2/%2_%3.jpg").arg( tilelevel + 1 ).arg( 2*n+1, 4, 10, QChar('0') ).arg( 2*m, 4, 10, QChar('0') );
				QImage img_bottomleft( tilename );
				
				tilename = destpath + QString("%1/%2/%2_%3.jpg").arg( tilelevel + 1 ).arg( 2*n+1, 4, 10, QChar('0') ).arg( 2*m+1, 4, 10, QChar('0') );
				QImage img_bottomright( tilename );

				QImage tile = img_topleft;
				tile.setColorTable( legpal );

				if ( tile.depth() == 8 ) {				
					uchar* destline;

					for ( int y=0; y < 338; ++y ){
						destline = tile.scanLine( y );
						const uchar* srcline = img_topleft.scanLine( 2*y );
						for ( int x=0; x < 338; ++x )
							destline[x]=srcline[2*x];
					}
					for ( int y=0; y < 338; ++y ){
						destline = tile.scanLine( y );
						const uchar* srcline = img_topright.scanLine( 2*y );
						for ( int x=338; x < 675; ++x )
							destline[x]=srcline[2*(x-338)];		
					}
					for ( int y=338; y < 675; ++y ){
						destline = tile.scanLine( y );
						const uchar* srcline = img_bottomleft.scanLine( 2 * ( y-338 ) );
						for ( int x=0; x < 338; ++x )
							destline[x]=srcline[2*x];	
					}
					for ( int y=338; y < 675; ++y ){
						destline = tile.scanLine( y );
						const uchar* srcline = img_bottomright.scanLine( 2 * ( y-338 ) );
						for ( int x=338; x < 675; ++x )
							destline[x]=srcline[2*(x-338)];		
					}
				}
				else {
					QRgb* destline;

					for ( int y=0; y < 338; ++y ){
						destline = (QRgb*) tile.scanLine( y );
						const QRgb* srcline = (QRgb*) img_topleft.scanLine( 2 * y );
						for ( int x=0; x < 338; ++x )
							destline[x]=srcline[2*x];
					}
					for ( int y=0; y < 338; ++y ){
						destline = (QRgb*) tile.scanLine( y );
						const QRgb* srcline = (QRgb*) img_topright.scanLine( 2 * y );
						for ( int x=338; x < 675; ++x )
							destline[x]=srcline[2*(x-338)];		
					}
					for ( int y=338; y < 675; ++y ){
						destline = (QRgb*) tile.scanLine( y );
						const QRgb* srcline = (QRgb*) img_bottomleft.scanLine( 2 * ( y-338 ) );
						for ( int x=0; x < 338; ++x )
							destline[x]=srcline[2*x];	
					}
					for ( int y=338; y < 675; ++y ){
						destline = (QRgb*) tile.scanLine( y );
						const QRgb* srcline = (QRgb*) img_bottomright.scanLine( 2 * ( y-338 ) );
						for ( int x=338; x < 675; ++x )
							destline[x]=srcline[2*(x-338)];		
					}
				}

				tilename = destpath + QString("%1/%2/%2_%3.jpg").arg( tilelevel ).arg( n, 4, 10, QChar('0') ).arg( m, 4, 10, QChar('0') );
				bool noerr = tile.save( tilename, "jpg", 100 );
				if ( noerr == false ) qDebug() << "Error while writing Tile: " << tilename;;

				completed = (int) ( 90 * (float)(count) / (float)(maxcount) );	
				count++;
						
				emit progress( completed );
				QApplication::processEvents(); 
			}
		}
		qDebug() << "Tilelevel: " << tilelevel << " successfully created.";

		tilelevel--;
	}
	qDebug() << "Tile creation completed.";
	// Applying correct JPEG compression
	int microcount = 0;
	int microcompleted = 0;

	tilelevel = 0;
	while ( tilelevel <= maxtilelevel ) {
		int nmaxit =  TileLoader::levelToRow( tilelevel );
		for ( int n=0; n < nmaxit; n++){
			int mmaxit =  TileLoader::levelToColumn( tilelevel );
			for ( int m=0; m < mmaxit; m++){ 
				microcount++;

				tilename = destpath + QString("%1/%2/%2_%3.jpg").arg( tilelevel ).arg( n, 4, 10, QChar('0') ).arg( m, 4, 10, QChar('0') );
				QImage tile( tilename );

				bool noerr = tile.save( tilename, "jpg", 85 );
				if ( noerr == false ) qDebug() << "Error while writing Tile: " << tilename;	

				microcompleted = (int) ( 100 * (float)(microcount) / (float)(maxcount) );	
				emit progress( 90 + (int)( 0.1*(float)microcompleted ) );
				QApplication::processEvents(); 
			}
		}
		tilelevel++;	
	}

	completed = 100;
	emit progress( 100 );
	QApplication::processEvents(); 

}

#ifndef Q_OS_MACX
#include "tilescissor.moc"
#endif
