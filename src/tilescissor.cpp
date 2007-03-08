#include "tilescissor.h"

#include <cmath>

#include <QtGui/QApplication>
#include <QtCore/QDebug>
#include <QtGui/QImage>
#include <QtGui/QImageReader>
#include <QtGui/QPainter>
#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtCore/QVector>

#include "katlasdirs.h"

TileScissor::TileScissor(const QString& prefix, const QString& installmap, const QString& dem):m_prefix(prefix), m_installmap(installmap),m_dem(dem){
	/* NOOP */
}

void TileScissor::createTiles() {

	QApplication::processEvents(); 

	QString srcpath = KAtlasDirs::path( "maps/" + m_prefix + "/" + m_installmap );
	QString destpath = KAtlasDirs::localDir() + "/maps/" + m_prefix + "/" 
		+ m_installmap.left( m_installmap.length()-4 );

	qDebug() << "creating tiles from: " << srcpath;
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

	int maxtilelevel = 0;
	int stdimgw = 0;
	int stdimgh = 0;

	int count = 0;
	while ( stdimgw < imgw ){
		maxtilelevel = (int)pow(2, (double)count);
		stdimgw = 2 * 675 * maxtilelevel;
		count++;
	}
	stdimgh = stdimgw / 2;
//	qDebug() << "MAXTILELEVEL=" << maxtilelevel;


	if ( QDir( KAtlasDirs::localDir() + "/maps/" + m_prefix ).exists() == false ) 
		( QDir::root() ).mkpath( KAtlasDirs::localDir() + "/maps/" + m_prefix );

	int tilelevel = 1;

	QSize stdsize = QSize( 675,675 );

	// Counting total amount of tiles to be generated for progressbar
	int maxcount = 0;
	while ( tilelevel <= maxtilelevel ) {
		for ( int n=0; n < 2*tilelevel; n++) 
			for ( int m=0; m < tilelevel; m++)
				maxcount++;
		tilelevel *= 2;	
	}

	// Loading each row at highest spatial resolution and croping tiles
	int completed = 0;
	count = 0; // reset, so that it counts the percentage correctly
	QString tilename;
	
	for ( int m=0; m < maxtilelevel; m++){
		QApplication::processEvents(); 

		QRect rowsrc( 0, (int)((float)(m*imgh)/(float)(maxtilelevel)), imgw, 	(int)((float)(imgh)/(float)(maxtilelevel)) );
/*
		// Unfortunately this code is out of order due to Qt 4.x regressions
		QImageReader img( srcpath );
		img.setClipRect( rowsrc );

		if ( smooth == true ){
			QSize destsize( stdimgw, 675 );
			img.setScaledSize( destsize );
			QRect destrow(  QPoint( 0, 0 ), destsize );
			img.setScaledClipRect( destrow );
		}
		QImage row = img.read();
		if ( row.isNull() ) qDebug() << "Read-Error: " << img.errorString();
*/
		QImage img( srcpath );

		QImage row = img.copy( rowsrc );
		QSize destsize( stdimgw, 675 );

		if ( smooth == true ){
			row = img.scaled(destsize,  Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		}

		if ( row.isNull() ) qDebug() << "Read-Error! Null QImage!";

		for ( int n=0; n < 2*maxtilelevel; n++) {
			QApplication::processEvents(); 

			QImage tile = row.copy( n*imgw/(2*maxtilelevel), 0, 675, 675 );

			tilename = destpath + QString("_%1_%2x%3.jpg").arg(maxtilelevel).arg(n).arg(m);

			if ( m_dem == "true" ){
				tile=tile.convertToFormat(QImage::Format_Indexed8, legpal, Qt::ThresholdDither);
			}

			bool noerr = tile.save( tilename, "jpg", 100);
			if ( noerr == false ) qDebug( "Error while writing Tile" );

			completed = (int) ( 90 * (float)(count) / (float)(maxcount) );	
			count++;
						
			emit progress( completed );
		}
	}

	tilelevel = maxtilelevel / 2;

	// Now that we have the tiles at the highest resolution lets build them together four by four

	while( tilelevel >= 1 ) {
		for ( int m=0; m < tilelevel; m++){
			for ( int n=0; n < 2*tilelevel; n++) {

				tilename = destpath + QString("_%1_%2x%3.jpg").arg(2*tilelevel).arg(2*n).arg(2*m);
				QImage img_topleft( tilename );
				
				tilename = destpath + QString("_%1_%2x%3.jpg").arg(2*tilelevel).arg(2*n+1).arg(2*m);
				QImage img_topright( tilename );

				tilename = destpath + QString("_%1_%2x%3.jpg").arg(2*tilelevel).arg(2*n).arg(2*m+1);
				QImage img_bottomleft( tilename );
				
				tilename = destpath + QString("_%1_%2x%3.jpg").arg(2*tilelevel).arg(2*n+1).arg(2*m+1);
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

				tilename = destpath + QString("_%1_%2x%3.jpg").arg(tilelevel).arg(n).arg(m);
				bool noerr = tile.save( tilename, "jpg", 100 );
				if ( noerr == false ) qDebug( "Error while writing Tile" );

				completed = (int) ( 90 * (float)(count) / (float)(maxcount) );	
				count++;
						
				emit progress( completed );
				QApplication::processEvents(); 
			}
		}
		tilelevel /= 2;
	}

	// Applying correct JPEG compression
	int microcount = 0;
	int microcompleted = 0;

	tilelevel = 1;
	while ( tilelevel <= maxtilelevel ) {
		for ( int m=0; m < tilelevel; m++){
			for ( int n=0; n < 2*tilelevel; n++){ 
				microcount++;

				tilename = destpath + QString("_%1_%2x%3.jpg").arg(tilelevel).arg(n).arg(m);
				QImage tile( tilename );

				bool noerr = tile.save( tilename, "jpg", 85 );
				if ( noerr == false ) qDebug( "Error while writing Tile" );	

				microcompleted = (int) ( 100 * (float)(microcount) / (float)(maxcount) );	
				emit progress( 90 + (int)( 0.1*(float)microcompleted ) );
				QApplication::processEvents(); 
			}
		}
		tilelevel *= 2;	
	}

	completed = 100;
	emit progress( 100 );
	QApplication::processEvents(); 

}

#include "tilescissor.moc"
