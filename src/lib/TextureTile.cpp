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

#include "TextureTile.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QFont>
#include <QtGui/QPainter>
#include <QtGui/QPen>

#include <cmath>

#include "MarbleDirs.h"
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

TextureTile::TextureTile( int id )
    : QObject(),
      jumpTable8(0),
      jumpTable32(0),
      m_id(id),
      m_rawtile( QImage() ),
      m_depth(0),
      m_used(false)/*,
      m_sun_shading(false)*/
{
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

//    qDebug() << "Tile deleted: " << m_id;
//    delete m_rawtile;
}


void TextureTile::loadTile( int x, int y, int level, 
			    const QString& theme, bool requestTileUpdate, bool sun_shading )
{
  //    qDebug() << "Entered loadTile( int, int, int) of Tile" << m_id;
  m_used = true; // Needed to avoid frequent deletion of tiles

  QString  absfilename;

  // qDebug() << "Requested tile level" << level;

  // If the tile level offers the requested tile then load it.
  // Otherwise cycle from the requested tilelevel down to one where
  // the requested area is covered.  Then scale the area to create a
  // replacement for the tile that has been requested.

  for ( int i = level; i > -1; --i ) {

    double origx1 = (double)(x) / (double)( TileLoader::levelToRow( level ) );
    double origy1 = (double)(y) / (double)( TileLoader::levelToColumn( level ) );
    double testx1 = origx1 * (double)( TileLoader::levelToRow( i ) ) ;
    double testy1 = origy1 * (double)( TileLoader::levelToColumn( i ) );

    QString relfilename = QString("%1/%2/%3/%3_%4.jpg")
        .arg(theme).arg(i)
        .arg( (int)(testy1), tileDigits, 10, QChar('0') )
        .arg( (int)(testx1), tileDigits, 10, QChar('0') );

    absfilename = MarbleDirs::path( relfilename );

    if ( QFile::exists( absfilename ) ) {
//       qDebug() << "The image filename does exist: " << absfilename ;

      QImage temptile( absfilename );

      if ( !temptile.isNull() ) {
//         qDebug() << "Image has been successfully loaded.";

        if ( level != i ) { 
//           qDebug() << "About to start cropping an existing image.";
          QSize tilesize = temptile.size();
          double origx2 = (double)(x + 1) / (double)( TileLoader::levelToRow( level ) );
          double origy2 = (double)(y + 1) / (double)( TileLoader::levelToColumn( level ) );
          double testx2 = origx2 * (double)( TileLoader::levelToRow( i ) );
          double testy2 = origy2 * (double)( TileLoader::levelToColumn( i ) );

          QPoint topleft( (int)( ( testx1 - (int)(testx1) ) * temptile.width() ),
                  (int)( ( testy1 - (int)(testy1) ) * temptile.height() ) );
          QPoint bottomright( (int)( ( testx2 - (int)(testx1) ) * temptile.width() ) - 1,
                  (int)( ( testy2 - (int)(testy1) ) * temptile.height() ) - 1 );

          // This should not create any memory leaks as
          // 'copy' and 'scaled' return a value (on the
          // stack) which gets deep copied always into the
          // same place for m_rawtile on the heap:
          temptile = temptile.copy( QRect( topleft, bottomright ) );
          temptile = temptile.scaled( tilesize ); // TODO: use correct size
//          qDebug() << "Finished scaling up the Temporary Tile.";
        }

        m_rawtile = temptile;

        break;
      } // !tempfile.isNull()
//      else {
//         qDebug() << "Image load failed for: " + 
//           absfilename.toLocal8Bit();
//      }
    }
    else {
//      qDebug() << "emit downloadTile(" << relfilename << ");";
      emit downloadTile( relfilename, QString::number( m_id ) );
    }
  }

  if ( m_rawtile.isNull() ) {
    qDebug() << "An essential tile is missing. Please rerun the application.";
    exit(-1);
  }

  m_worktile = m_rawtile;
  m_depth = m_worktile.depth();

  // FIXME: This should get accessible from MarbleWidget, so we can pass over 
  //        a testing command line option
  bool tileIdVisible = false;

  if( tileIdVisible ) {
    showTileId( m_worktile, theme, level, x, y );
  }

  if(sun_shading && m_depth == 32) {
  // TODO add support for 8-bit maps?
  // add sun shading
  m_sun.updatePosition();
  const double global_width = m_worktile.width() * TileLoader::levelToColumn(level);
  const double global_height = m_worktile.height() * TileLoader::levelToRow(level);
  const double lon_scale = 2*M_PI / global_width;
  const double lat_scale = -M_PI / global_height;
  const int tileHeight = m_worktile.height();
  const int tileWidth = m_worktile.width();
  for(int cur_y = 0; cur_y < tileHeight; cur_y++) {
    double lat = lat_scale * (y * tileHeight + cur_y) - 0.5*M_PI;
    QRgb* scanline = (QRgb*)m_worktile.scanLine(cur_y);
    for(int cur_x = 0; cur_x < tileWidth; cur_x++) {
      double lon = lon_scale * (x * tileWidth + cur_x);
      m_sun.shadePixel(*scanline, m_sun.shading(lat, lon));
      scanline++;
    }
  }
  }

  switch ( m_depth ) {
      case 32:
          if ( jumpTable32 ) delete [] jumpTable32;
          jumpTable32 = jumpTableFromQImage32( m_worktile );
          break;
      case 8:
          if ( jumpTable8 ) delete [] jumpTable8;
          jumpTable8 = jumpTableFromQImage8( m_worktile );
          break;
      default:
          qDebug() << QString("Color m_depth %1 of tile %2 could not be retrieved. Exiting.").arg(m_depth).arg(absfilename);
          exit( -1 );
  }

  if ( requestTileUpdate ) {
    // qDebug() << "TileUpdate available";
    emit tileUpdateDone();
  }
}


void TextureTile::reloadTile( int x, int y, int level, const QString& theme, bool sun_shading )
{
    // qDebug() << "slotLoadTile variables: |" << theme << "|" 
    // << level << "|" << x << "|" << y;

    loadTile( x, y, level, theme, true, sun_shading );
}

void TextureTile::showTileId( QImage& worktile, QString theme, int level, int x, int y )
{
    QString filename = QString("%1_%2.jpg")
        .arg( x, tileDigits, 10, QChar('0') )
        .arg( y, tileDigits, 10, QChar('0') );

    QPainter painter(&m_worktile);

    QColor foreground;
    QColor background;

    if ( ( (double)(x)/2 == x/2 && (double)(y)/2 == y/2 ) || 
         ( (double)(x)/2 != x/2 && (double)(y)/2 != y/2 ) 
       )
    {
        foreground.setNamedColor("#FFFFFF");
        background.setNamedColor("#000000");
    }
    else
    {
        foreground.setNamedColor("#000000");
        background.setNamedColor("#FFFFFF");
    }

    int strokeWidth = 10;
    QPen testPen( foreground );
    testPen.setWidth( strokeWidth );
    testPen.setJoinStyle(Qt::MiterJoin);

    painter.setPen( testPen );
    painter.drawRect( strokeWidth / 2, strokeWidth / 2, 
                      worktile.width()  - strokeWidth,
                      worktile.height() - strokeWidth
    );
    QFont testFont("Sans", 30, QFont::Bold);
    QFontMetrics testFm(testFont);
    painter.setFont(testFont);

    QPen outlinepen( foreground );
    outlinepen.setWidthF( 6 );

    painter.setPen( outlinepen );
    painter.setBrush( background );

    QPainterPath   outlinepath;

    QPointF  baseline1( ( worktile.width() - testFm.boundingRect(filename).width() ) / 2,
                             (worktile.height() * 0.25) );
    outlinepath.addText( baseline1, testFont, QString( "level: %1" ).arg(level) );

    QPointF  baseline2( ( worktile.width() - testFm.boundingRect(filename).width() ) / 2,
                             (worktile.height() * 0.50) );
    outlinepath.addText( baseline2, testFont, filename );

    QPointF  baseline3( ( worktile.width() - testFm.boundingRect(filename).width() ) / 2,
                             (worktile.height() * 0.75) );
    outlinepath.addText( baseline3, testFont, theme );

    painter.drawPath( outlinepath );

    painter.setPen( Qt::NoPen );
    painter.drawPath( outlinepath );
}

#include "TextureTile.moc"
