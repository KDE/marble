// (c) 2008 David Roberts

#include "MergedLayerPainter.h"

#include <QtCore/QFile>
#include <QDebug>
#include <QPainter>

#include "MarbleDirs.h"
#include "TileLoader.h"


MergedLayerPainter::MergedLayerPainter()
{
}

MergedLayerPainter::~MergedLayerPainter()
{
}

void MergedLayerPainter::paint(const QString& theme)
{
    *m_tile = loadRawTile( theme );
}

void MergedLayerPainter::paintClouds()
{
    QImage  cloudtile = loadRawTile( "maps/earth/clouds" );
    if ( cloudtile.isNull() )
        return;
	
    const int  ctileHeight = cloudtile.height();
    const int  ctileWidth  = cloudtile.width();
	
    for ( int cur_y = 0; cur_y < ctileHeight; ++cur_y ) {
        QRgb  *cscanline = (QRgb*)cloudtile.scanLine( cur_y );
        QRgb  *scanline  = (QRgb*)m_tile->scanLine( cur_y );
        for ( int cur_x = 0; cur_x < ctileWidth; ++cur_x ) {
            double  c   = qRed( *cscanline ) / 255.0;
            QRgb    pix = *scanline;
            int  r = qRed( pix );
            int  g = qGreen( pix );
            int  b = qBlue( pix );
            *scanline = qRgb( (int)( r + (255-r)*c ),
                              (int)( g + (255-g)*c ),
                              (int)( b + (255-b)*c ) );
            cscanline++;
            scanline++;
        }
    }
}

void MergedLayerPainter::paintSunShading(SunLocator* sunLocator)
{
    if ( m_tile->depth() != 32 )
        return;
	
    // FIXME: This should get accessible from MarbleWidget, so we can pass over 
    //        a testing command line option
    // 	bool tileIdVisible = false;
    // 	
    // 	if( tileIdVisible ) {
    // 	showTileId( m_worktile, theme, m_level, x, y );
    // 	}
	
    // TODO add support for 8-bit maps?
    // add sun shading
    const double  global_width  = m_tile->width() * TileLoader::levelToColumn(m_level);
    const double  global_height = m_tile->height() * TileLoader::levelToRow(m_level);
    const double lon_scale = 2*M_PI / global_width;
    const double lat_scale = -M_PI / global_height;
    const int tileHeight = m_tile->height();
    const int tileWidth = m_tile->width();

    // First we determine the supporting point interval for the interpolation.
    const int n = maxDivisor( 30, tileWidth );
    const double nInverse = 1.0 / (double)(n);
    const int ipRight = n * (int)( tileWidth / n );

    if ( sunLocator->getCitylights() ) {
        QImage nighttile = loadRawTile( "maps/earth/citylights" );
        if ( nighttile.isNull() )
            return;
        for ( int cur_y = 0; cur_y < tileHeight; ++cur_y ) {
            double lat = lat_scale * ( m_y * tileHeight + cur_y ) - 0.5*M_PI;
            QRgb* scanline  = (QRgb*)m_tile->scanLine( cur_y );
            QRgb* nscanline = (QRgb*)nighttile.scanLine( cur_y );
            double lastShade = 0.0;

            for ( int cur_x = 0; cur_x < tileWidth; ++cur_x) {

                bool interpolate = ( cur_x != 0 && cur_x < ipRight );

                if ( interpolate == true ) cur_x+= n - 1;

                double lon   = lon_scale * (m_x * tileWidth + cur_x);
                double shade = sunLocator->shading( lat, lon );

                if ( interpolate == true ) {

                    // if the shading didn't change across the interpolation
                    // interval move on and don't change anything.
                    if ( shade == lastShade && shade == 1.0 ) {
                        scanline += n;
                        nscanline += n;
                        continue;
                    }
                    else {
                        double interpolatedShade = lastShade;
                        const double shadeDiff = ( shade - lastShade ) * nInverse;

                        // Now we do linear interpolation across the tile width
                        for ( int t = 1; t < n; ++t )
                        {
                            interpolatedShade += shadeDiff;
                            sunLocator->shadePixelComposite( *scanline, *nscanline, interpolatedShade );
                            scanline++;
                            nscanline++;
                        }
                    }
                }

                // Make sure we don't exceed the image memory
                if ( cur_x < tileWidth ) {
                    sunLocator->shadePixelComposite( *scanline, *nscanline, shade );
                    scanline++;
                    nscanline++;
                }

                lastShade = shade;
            }
        }
    } else {
        for ( int cur_y = 0; cur_y < tileHeight; ++cur_y ) {
            double lat = lat_scale * (m_y * tileHeight + cur_y) - 0.5*M_PI;
            QRgb* scanline = (QRgb*)m_tile->scanLine( cur_y );

            double lastShade = 0.0;

            for ( int cur_x = 0; cur_x <= tileWidth; ++cur_x ) {

                bool interpolate = ( cur_x != 0 && cur_x < ipRight );

                if ( interpolate == true ) cur_x+= n - 1;

                double lon   = lon_scale * ( m_x * tileWidth + cur_x );
                double shade = sunLocator->shading(lat, lon);

                if ( interpolate == true ) {

                    // if the shading didn't change across the interpolation
                    // interval move on and don't change anything.
                    if ( shade == lastShade && shade == 1.0 ) {
                        scanline += n;
                        continue;
                    }
                    else {
                        double interpolatedShade = lastShade;
                        const double shadeDiff = ( shade - lastShade ) * nInverse;

                        // Now we do linear interpolation across the tile width
                        for ( int t = 1; t < n; ++t )
                        {
                            interpolatedShade += shadeDiff;
                            sunLocator->shadePixel( *scanline, interpolatedShade );
                            scanline++;
                        }
                    }
                }

                // Make sure we don't exceed the image memory
                if ( cur_x < tileWidth ) {
                    sunLocator->shadePixel( *scanline, shade );
                    scanline++;
                }

                lastShade = shade;
            }
        }
    }
}

void MergedLayerPainter::paintTileId(const QString& theme)
{
    QString filename = QString("%1_%2.jpg")
        .arg( m_x, tileDigits, 10, QChar('0') )
        .arg( m_y, tileDigits, 10, QChar('0') );

    QPainter painter(m_tile);

    QColor foreground;
    QColor background;

    if ( ( (double)(m_x)/2 == m_x/2 && (double)(m_y)/2 == m_y/2 ) 
         || ( (double)(m_x)/2 != m_x/2 && (double)(m_y)/2 != m_y/2 ) 
       )
    {
        foreground.setNamedColor("#FFFFFF");
        background.setNamedColor("#000000");
    }
    else {
        foreground.setNamedColor("#000000");
        background.setNamedColor("#FFFFFF");
    }

    int   strokeWidth = 10;
    QPen  testPen( foreground );
    testPen.setWidth( strokeWidth );
    testPen.setJoinStyle(Qt::MiterJoin);

    painter.setPen( testPen );
    painter.drawRect( strokeWidth / 2, strokeWidth / 2, 
                      m_tile->width()  - strokeWidth,
                      m_tile->height() - strokeWidth
    );
    QFont testFont("Sans", 30, QFont::Bold);
    QFontMetrics testFm( testFont );
    painter.setFont( testFont );

    QPen outlinepen( foreground );
    outlinepen.setWidthF( 6 );

    painter.setPen( outlinepen );
    painter.setBrush( background );

    QPainterPath   outlinepath;

    QPointF  baseline1( ( m_tile->width() - testFm.boundingRect(filename).width() ) / 2,
                        ( m_tile->height() * 0.25) );
    outlinepath.addText( baseline1, testFont, QString( "level: %1" ).arg(m_level) );

    QPointF  baseline2( ( m_tile->width() - testFm.boundingRect(filename).width() ) / 2,
                        m_tile->height() * 0.50 );
    outlinepath.addText( baseline2, testFont, filename );

    QPointF  baseline3( ( m_tile->width() - testFm.boundingRect(filename).width() ) / 2,
                        m_tile->height() * 0.75 );
    outlinepath.addText( baseline3, testFont, theme );

    painter.drawPath( outlinepath );

    painter.setPen( Qt::NoPen );
    painter.drawPath( outlinepath );
}

void MergedLayerPainter::setTile(QImage* tile)
{
    m_tile = tile;
}

void MergedLayerPainter::setInfo(int x, int y, int level, int id)
{
    m_x = x;
    m_y = y;
    m_level = level;
    m_id = id;
}

QImage MergedLayerPainter::loadRawTile(const QString& theme)
{
    // qDebug() << "Entered loadTile( int, int, int) of Tile" << m_id;
    // m_used = true; // Needed to avoid frequent deletion of tiles

  QString  absfilename;

  // qDebug() << "Requested tile level" << level;

  // If the tile level offers the requested tile then load it.
  // Otherwise cycle from the requested tilelevel down to one where
  // the requested area is covered.  Then scale the area to create a
  // replacement for the tile that has been requested.

  for ( int i = m_level; i > -1; --i ) {

      double origx1 = (double)(m_x) / (double)( TileLoader::levelToRow( m_level ) );
      double origy1 = (double)(m_y) / (double)( TileLoader::levelToColumn( m_level ) );
      double testx1 = origx1 * (double)( TileLoader::levelToRow( i ) ) ;
      double testy1 = origy1 * (double)( TileLoader::levelToColumn( i ) );

      QString relfilename = QString("%1/%2/%3/%3_%4.jpg")
          .arg(theme).arg(i)
          .arg( (int)(testy1), tileDigits, 10, QChar('0') )
          .arg( (int)(testx1), tileDigits, 10, QChar('0') );

      absfilename = MarbleDirs::path( relfilename );

      if ( QFile::exists( absfilename ) ) {
          // qDebug() << "The image filename does exist: " << absfilename ;

          QImage temptile( absfilename );

          if ( !temptile.isNull() ) {
              //         qDebug() << "Image has been successfully loaded.";

              if ( m_level != i ) { 
                  // qDebug() << "About to start cropping an existing image.";
                  QSize tilesize = temptile.size();
                  double origx2 = (double)(m_x + 1) / (double)( TileLoader::levelToRow( m_level ) );
                  double origy2 = (double)(m_y + 1) / (double)( TileLoader::levelToColumn( m_level ) );
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

              return temptile;

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
  
  return QImage();
}

// FIXME: This should likely go into a math class in the future ...

int MergedLayerPainter::maxDivisor( int maximum, int fullLength )
{
    // Find the optimal interpolation interval n for the 
    // current image canvas width
    int best = 2;

    int  nEvalMin = fullLength;
    for ( int it = 1; it <= maximum; ++it ) {
        // The optimum is the interval which results in the least amount
        // supporting points taking into account the rest which can't 
        // get used for interpolation.
        int nEval = fullLength / it + fullLength % it;
        if ( nEval < nEvalMin ) {
            nEvalMin = nEval;
            best = it; 
        }
    }
    return best;
}

#include "MergedLayerPainter.moc"
