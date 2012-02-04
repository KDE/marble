//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DOWNLOADREGION_H
#define MARBLE_DOWNLOADREGION_H

#include <QtCore/QObject>
#include <QtCore/QVector>

#include "TileCoordsPyramid.h"
#include "marble_export.h"

namespace Marble
{
class DownloadRegionPrivate;
class GeoDataLatLonAltBox;
class MarbleWidget;
class ViewportParams;

class MARBLE_EXPORT DownloadRegion : public QObject
{
    Q_OBJECT

 public:
    DownloadRegion( QObject* parent=0 );

    void setMarbleWidget( MarbleWidget* map );

    ~DownloadRegion();

    void setTileLevelRange( int const minimumTileLevel, int const maximumTileLevel );

    QVector<TileCoordsPyramid> region( const GeoDataLatLonAltBox &region ) const;

    void setVisibleTileLevel( int const tileLevel );

    /**
      * @brief calculates the region to be downloaded around a route
      */
    QVector<TileCoordsPyramid> routeRegion( qreal offset ) const;

  private:
    DownloadRegionPrivate* const d;
};

}

#endif
