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
    QList< GeoGraphicsItem* > result;
    for( QMap< TileId, QList< GeoGraphicsItem* > >::const_iterator i = d->m_items.constBegin(); 
         i != d->m_items.constEnd(); i++ )
    {
        const QList< GeoGraphicsItem* > &objects = *i;
        QList< GeoGraphicsItem* >::iterator before = result.begin();
        QList< GeoGraphicsItem* >::const_iterator currentItem = objects.constBegin();
        while( currentItem != objects.end() )
        {
            while( ( currentItem != objects.end() )
                  && ( ( before == result.end() ) || ( (*currentItem)->zValue() < (*before)->zValue() ) ) )
            {
                before = result.insert( before, *currentItem );
                currentItem++;
            }
            if ( before != result.end() )
                before++;
         }
    }
    return result;
}

QList< GeoGraphicsItem* > GeoGraphicsScene::items( const Marble::GeoDataLatLonAltBox& box, int maxZoomLevel ) const
{
    QList< GeoGraphicsItem* > result;
    QRect rect;
    qreal north, south, east, west;
    box.boundaries( north, south, east, west );
    TileId key;
    int zoomLevel = maxZoomLevel < s_tileZoomLevel ? maxZoomLevel : s_tileZoomLevel;

    key = d->coordToTileId( GeoDataCoordinates(west, north, 0), zoomLevel );
    rect.setLeft( key.x() );
    rect.setTop( key.y() );

    key = d->coordToTileId( GeoDataCoordinates(east, south, 0), zoomLevel );
    rect.setRight( key.x() );
    rect.setBottom( key.y() );
    
    TileCoordsPyramid pyramid( 0, zoomLevel );
    pyramid.setBottomLevelCoords( rect );

    for ( int level = pyramid.topLevel(); level <= pyramid.bottomLevel(); ++level ) {
        QRect const coords = pyramid.coords( level );
        int x1, y1, x2, y2;
        coords.getCoords( &x1, &y1, &x2, &y2 );
        for ( int x = x1; x <= x2; ++x ) {
            for ( int y = y1; y <= y2; ++y ) {
                TileId const tileId( "", level, x, y );
                const QList< GeoGraphicsItem* > &objects = d->m_items.value(tileId);
                QList< GeoGraphicsItem* >::iterator before = result.begin();
                QList< GeoGraphicsItem* >::const_iterator currentItem = objects.constBegin();
                while( currentItem != objects.end() )
                {
                    while( ( currentItem != objects.end() )
                      && ( ( before == result.end() ) || ( (*currentItem)->zValue() < (*before)->zValue() ) ) )
                    {
                        before = result.insert( before, *currentItem );
                        currentItem++;
                    }
                    if ( before != result.end() )
                        before++;
                }
            }
        }
    }
    return result;
}

void GeoGraphicsScene::removeItem( GeoGraphicsItem* item )
{
    int zoomLevel;
    qreal north, south, east, west;
    item->latLonAltBox().boundaries( north, south, east, west );
    for(zoomLevel = s_tileZoomLevel; zoomLevel >= 0; zoomLevel--)
    {
        if( d->coordToTileId( GeoDataCoordinates(west, north, 0), zoomLevel ) == 
            d->coordToTileId( GeoDataCoordinates(east, south, 0), zoomLevel ) )
            break;
    }
    int tnorth, tsouth, teast, twest;
    TileId key;
    
    key = d->coordToTileId( GeoDataCoordinates(west, north, 0), zoomLevel );
    twest = key.x();
    tnorth = key.y();

    key = d->coordToTileId( GeoDataCoordinates(east, south, 0), zoomLevel );
    teast = key.x();
    tsouth = key.y();
        
    for( int i = twest; i <= teast; i++ )
    {
        for( int j = tsouth; j <= tnorth; j++ )
        {
            QList< GeoGraphicsItem* >& tileList = d->m_items[TileId( "", zoomLevel, i, j )]; 
            tileList.removeOne( item );
        }
    }
}

void GeoGraphicsScene::clear()
{
    d->m_items.clear();
}

void GeoGraphicsScene::addIdem( GeoGraphicsItem* item )
{
    // Select zoom level so that the object fit in single tile
    int zoomLevel;
    qreal north, south, east, west;
    item->latLonAltBox().boundaries( north, south, east, west );
    for(zoomLevel = s_tileZoomLevel; zoomLevel >= 0; zoomLevel--)
    {
        if( d->coordToTileId( GeoDataCoordinates(west, north, 0), zoomLevel ) == 
            d->coordToTileId( GeoDataCoordinates(east, south, 0), zoomLevel ) )
            break;
    }
    int tnorth, tsouth, teast, twest;
    TileId key;
    
    key = d->coordToTileId( GeoDataCoordinates(west, north, 0), zoomLevel );
    twest = key.x();
    tnorth = key.y();

    key = d->coordToTileId( GeoDataCoordinates(east, south, 0), zoomLevel );
    teast = key.x();
    tsouth = key.y();
        
    for( int i = twest; i <= teast; i++ )
    {
        for( int j = tsouth; j <= tnorth; j++ )
        {
            QList< GeoGraphicsItem* >& tileList = d->m_items[TileId( "", zoomLevel, i, j )]; 
            QList< GeoGraphicsItem* >::iterator position = qLowerBound( tileList.begin(), tileList.end(), item, zValueLessThan );
            tileList.insert( position, item );
        }
    }
}
};

#include "GeoGraphicsScene.moc"
