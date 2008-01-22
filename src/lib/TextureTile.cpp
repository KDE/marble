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
      m_used(false)
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
			    const QString& theme, bool requestTileUpdate, SunLocator* sunLocator )
{
  //    qDebug() << "Entered loadTile( int, int, int) of Tile" << m_id;
  m_used = true; // Needed to avoid frequent deletion of tiles

  m_painter.setInfo(x, y, level, m_id);

  m_painter.setTile(&m_rawtile);
  m_painter.paint(theme);

  if ( m_rawtile.isNull() ) {
    qDebug() << "An essential tile is missing. Please rerun the application.";
    exit(-1);
  }

  m_worktile = m_rawtile;
  m_depth = m_worktile.depth();
  m_painter.setTile(&m_worktile);

  // TODO be able to set this somewhere
  bool cloudlayer = true; if(cloudlayer && m_depth == 32 && level < 2) m_painter.paintClouds();

  if(sunLocator != 0 && sunLocator->getShow()) m_painter.paintSunShading(sunLocator);

  // FIXME: This should get accessible from MarbleWidget, so we can pass over 
  //        a testing command line option
  bool tileIdVisible = false; if(tileIdVisible) m_painter.paintTileId(theme);

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
          qDebug() << QString("Color m_depth %1 of tile could not be retrieved. Exiting.").arg(m_depth);
          exit( -1 );
  }

  if ( requestTileUpdate ) {
    // qDebug() << "TileUpdate available";
    emit tileUpdateDone();
  }
}

void TextureTile::reloadTile( int x, int y, int level, const QString& theme, SunLocator* sunLocator )
{
    // qDebug() << "slotLoadTile variables: |" << theme << "|" 
    // << level << "|" << x << "|" << y;

    loadTile( x, y, level, theme, true, sunLocator );
}

#include "TextureTile.moc"
