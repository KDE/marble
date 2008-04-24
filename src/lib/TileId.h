//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Jens-Michael Hoffmann  <jensmh@gmx.de>"
//

#ifndef MARBLE_TILE_ID_H
#define MARBLE_TILE_ID_H

#include <QtCore/QString>


class TileId
{
 public:
  TileId( int zoomLevel, int tileX, int tileY );
  TileId();

  int zoomLevel() const;
  int x() const;
  int y() const;

  QString toString() const;
  static TileId fromString( QString const& );

 private:
  int m_zoomLevel;
  int m_tileX;
  int m_tileY;
};


bool operator<( TileId const& lhs, TileId const& rhs );
bool operator==( TileId const& lhs, TileId const& rhs );
uint qHash( TileId const& );


#endif
