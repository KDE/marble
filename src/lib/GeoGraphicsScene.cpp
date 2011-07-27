//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "GeoGraphicsScene.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoGraphicsItem.h"
#include "TileId.h"
#include "TileCoordsPyramid.h"
#include "MarbleDebug.h"
#include <QtCore/QMap>

namespace Marble
{

bool zValueLessThan( GeoGraphicsItem* i1, GeoGraphicsItem* i2 )
{
    return i1->zValue() < i2->zValue();
}

int GeoGraphicsScene::s_tileZoomLevel = 14;

class GeoGraphicsScenePrivate
{
public:
    TileId coordToTileId( const GeoDataCoordinates& coord, int popularity ) const
    {
        if ( popularity < 0 ) {
            return TileId();
        }
        int maxLat = 90000000;
        int maxLon = 180000000;
        int lat = coord.latitude( GeoDataCoordinates::Degree ) * 1000000;
        int lon = coord.longitude( GeoDataCoordinates::Degree ) * 1000000;
        int deltaLat, deltaLon;
        int x = 0;
        int y = 0;
        for( int i=0; i<popularity; ++i ) {
            deltaLat = maxLat >> i;
            if( lat < ( maxLat - deltaLat )) {
                y += 1<<(popularity-i-1);
                lat += deltaLat;
            }
            deltaLon = maxLon >> i;
            if( lon >= ( maxLon - deltaLon )) {
                x += 1<<(popularity-i-1);
            } else {
                lon += deltaLon;
            }
        }
        return TileId( "", popularity, x, y );
    }

    QMap<TileId, QList<GeoGraphicsItem*> > m_items;
};

GeoGraphicsScene::GeoGraphicsScene( QObject* parent ): QObject( parent ), d( new GeoGraphicsScenePrivate() )
{

}

GeoGraphicsScene::~GeoGraphicsScene()
{
    delete d;
}

QList< GeoGraphicsItem* > GeoGraphicsScene::items() const
{
    //TODO: insert items
    return QList< GeoGraphicsItem* >();
    //return d->m_items;
}

QList< GeoGraphicsItem* > GeoGraphicsScene::items( const Marble::GeoDataLatLonAltBox& box ) const
{
    QList< GeoGraphicsItem* > result;
    QRect rect;
    qreal north, south, east, west;
    box.boundaries( north, south, east, west );
    TileId key;

    key = d->coordToTileId( GeoDataCoordinates(west, north, 0), s_tileZoomLevel );
    rect.setLeft( key.x() );
    rect.setTop( key.y() );

    key = d->coordToTileId( GeoDataCoordinates(east, south, 0), s_tileZoomLevel );
    rect.setRight( key.x() );
    rect.setBottom( key.y() );
    
    TileCoordsPyramid pyramid( s_tileZoomLevel, s_tileZoomLevel );
    pyramid.setBottomLevelCoords( rect );

    for ( int level = pyramid.topLevel(); level <= pyramid.bottomLevel(); ++level ) {
        QRect const coords = pyramid.coords( level );
        int x1, y1, x2, y2;
        coords.getCoords( &x1, &y1, &x2, &y2 );
        for ( int x = x1; x <= x2; ++x ) {
            for ( int y = y1; y <= y2; ++y ) {
                TileId const tileId( "", level, x, y );
                result += d->m_items.value(tileId);
            }
        }
    }
    //TODO: Even quicksort isn't fast enouth... 
    qSort( result.begin(), result.end(), zValueLessThan );
    return result;
}

void GeoGraphicsScene::removeItem( GeoGraphicsItem* item )
{
    //TODO: Remove one item
    //d->m_items.removeOne( item );
}

void GeoGraphicsScene::clear()
{
    d->m_items.clear();
}

void GeoGraphicsScene::addIdem( GeoGraphicsItem* item )
{

    TileId key = d->coordToTileId( item->coordinate(), s_tileZoomLevel );
    QList< GeoGraphicsItem* >& tileList = d->m_items[key]; 
    QList< GeoGraphicsItem* >::iterator position = qLowerBound( tileList.begin(), tileList.end(), item, zValueLessThan );
    tileList.insert( position, item );
}
};

#include "GeoGraphicsScene.moc"
