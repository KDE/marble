#include "texturetile.h"

#include <QtCore/QDebug>

#include "katlasdirs.h"
#include "TileLoader.h"


static uint **jumpTableFromQImage32( QImage &img )
{
    const int  height = img.height();
    const int  bpl    = img.bytesPerLine() / 4;
    uint      *data = (QRgb*)(img.scanLine(0));
    uint     **jumpTable = new uint*[height];

    for ( int y = 0; y < height; ++y ) {
	jumpTable[ y ] = data;
	data += bpl;
    }

    return jumpTable;
}


static uchar **jumpTableFromQImage8( QImage &img )
{
    const int  height = img.height();
    const int  bpl    = img.bytesPerLine();
    uchar     *data = img.bits();
    uchar    **jumpTable = new uchar*[height];

    for ( int y = 0; y < height; ++y ) {
	jumpTable[ y ] = data;
	data += bpl;
    }

    return jumpTable;
}


TextureTile::TextureTile( int x, int y, int level, const QString& theme )
    : QObject()
{
    loadTile( x, y, level, theme, false );
}


void TextureTile::loadTile( int x, int y, int level, 
			     const QString& theme, bool requestRepaint )
{
    m_used = true;

    QString  absfilename;
    // qDebug() << "Requested tile level" << level;

    // If the tile level offers the requested tile then load it.
    // Otherwise cycle from the requested tilelevel down to one where
    // the requested area is covered.  Then scale the area to create a
    // replacement for the tile that has been requested.

    for ( int i = level; i > -1; --i ) {

	float origx1 = (float)(x) / (float)( TileLoader::levelToRow( level ) );
	float origy1 = (float)(y) / (float)( TileLoader::levelToColumn( level ) );
	float testx1 = origx1 * (float)( TileLoader::levelToRow( i ) ) ;
	float testy1 = origy1 * (float)( TileLoader::levelToColumn( i ) );

	QString relfilename = QString("%1/%2/%3/%3_%4.jpg").arg(theme).arg(i).arg( (int)(testy1), 4, 10, QChar('0') ).arg( (int)(testx1), 4, 10, QChar('0') );

	absfilename = KAtlasDirs::path( relfilename );
	if ( QFile::exists( absfilename ) ){
	    m_rawtile = new QImage( absfilename );
	    // qDebug() << absfilename;
	    if ( !m_rawtile->isNull() ) {

		if ( level != i ) { 
		    QSize tilesize = m_rawtile->size();
		    float origx2 = (float)(x + 1) / (float)( TileLoader::levelToRow( level ) );
		    float origy2 = (float)(y + 1) / (float)( TileLoader::levelToColumn( level ) );
		    float testx2 = origx2 * (float)( TileLoader::levelToRow( i ) );
		    float testy2 = origy2 * (float)( TileLoader::levelToColumn( i ) );
	
		    QPoint topleft( (int)( ( testx1 - (int)(testx1) ) * m_rawtile->width() ), (int)( ( testy1 - (int)(testy1) ) * m_rawtile->height() ) );
		    QPoint bottomright( (int)( ( testx2 - (int)(testx1) ) * m_rawtile->width() ) - 1, (int)( ( testy2 - (int)(testy1) ) * m_rawtile->height() ) - 1 );
				
		    //					qDebug() << "x1: " << topleft.x() << "y1: " << topleft.y() << "x2: " << bottomright.x() << "y2: " << bottomright.y();  
		    *m_rawtile = m_rawtile->copy( QRect( topleft, bottomright ) );
		    *m_rawtile = m_rawtile->scaled( tilesize ); // TODO: use correct size
		}

		break;
	    }
	}
	else {
	    emit downloadTile( relfilename );
	}
    }
	
    if ( m_rawtile->isNull() ){
	qDebug() << "An essential tile is missing. Please rerun the application.";
	exit(-1);
    }


    m_depth = m_rawtile->depth();

    switch ( m_depth ) {
    case 32:
	// qDebug("32");
	jumpTable32=jumpTableFromQImage32(*m_rawtile);
	break;
    case 8:
	// qDebug("8");
	jumpTable8=jumpTableFromQImage8(*m_rawtile);
	break;
    default:
	qDebug() << QString("Color m_depth %1 of tile %2 could not be retrieved. Exiting.").arg(m_depth).arg(absfilename);
	exit(-1);
    }

    if ( requestRepaint == true )
	emit tileUpdate();
}


TextureTile::~TextureTile()
{
    switch ( m_depth ) {
    case 32:
	delete [] jumpTable32;
	break;
    case 8:
	delete [] jumpTable8;
	break;
    default:
	qDebug("Color m_depth of a tile could not be retrieved. Exiting.");
	exit(-1);
    }

    delete m_rawtile;
}

void TextureTile::slotLoadTile( const QString& path )
{
    // QString relfilename = QString("%1/%2/%3/%3_%4.jpg").arg(theme).arg(i).arg( (int)(testy1), 4, 10, QChar('0') ).arg( (int)(testx1), 4, 10, QChar('0') );
    QString filename = path.section( '/', -1 );
    int x = filename.section( '_', 0, 0 ).toInt();
    int y = filename.section( '_', 1, 1 ).section( '.', 0, 0 ).toInt();
    int level = path.section( '/', 1, 1 ).toInt();
    QString theme = path.section( '/', 0, 0 );

    //	qDebug() << "Test: |" << theme << "|" << level << "|" << x << "|" << y;

    loadTile( x, y, level, theme, true );
}


#ifndef Q_OS_MACX
#include "texturetile.moc"
#endif
