//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010,2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

// Own
#include "ServerLayout.h"

#include "GeoSceneTileDataset.h"
#include "GeoDataLatLonBox.h"
#include "MarbleGlobal.h"
#include "TileId.h"

#include <QUrlQuery>

#include <math.h>

namespace Marble
{

ServerLayout::ServerLayout( GeoSceneTileDataset *textureLayer )
    : m_textureLayer( textureLayer )
{
}

ServerLayout::~ServerLayout()
{
}

MarbleServerLayout::MarbleServerLayout( GeoSceneTileDataset *textureLayer )
    : ServerLayout( textureLayer )
{
}

QUrl MarbleServerLayout::downloadUrl( const QUrl &prototypeUrl, const TileId &id ) const
{
    const QString path = QString( "%1/%2/%3/%3_%4.%5" )
        .arg( prototypeUrl.path() )
        .arg( id.zoomLevel() )
        .arg(id.y(), tileDigits, 10, QLatin1Char('0'))
        .arg(id.x(), tileDigits, 10, QLatin1Char('0'))
        .arg( m_textureLayer->fileFormat().toLower() );

    QUrl url = prototypeUrl;
    url.setPath( path );

    return url;
}

QString MarbleServerLayout::name() const
{
    return "Marble";
}

QString ServerLayout::sourceDir() const
{
    return m_textureLayer ? m_textureLayer->sourceDir() : QString();
}


OsmServerLayout::OsmServerLayout( GeoSceneTileDataset *textureLayer )
    : ServerLayout( textureLayer )
{
}

QUrl OsmServerLayout::downloadUrl( const QUrl &prototypeUrl, const TileId &id ) const
{
    const QString suffix = m_textureLayer->fileFormat().toLower();
    const QString path = QString( "%1/%2/%3.%4" ).arg( id.zoomLevel() )
                                                 .arg( id.x() )
                                                 .arg( id.y() )
                                                 .arg( suffix );

    QUrl url = prototypeUrl;
    url.setPath( url.path() + path );

    return url;
}

QString OsmServerLayout::name() const
{
    return "OpenStreetMap";
}


CustomServerLayout::CustomServerLayout( GeoSceneTileDataset *texture )
    : ServerLayout( texture )
{
}

QUrl CustomServerLayout::downloadUrl( const QUrl &prototypeUrl, const TileId &id ) const
{
    GeoDataLatLonBox bbox;
    m_textureLayer->tileProjection()->geoCoordinates(id, bbox);

    QString urlStr = prototypeUrl.toString( QUrl::DecodeReserved );

    urlStr.replace( "{zoomLevel}", QString::number( id.zoomLevel() ) );
    urlStr.replace( "{x}", QString::number( id.x() ) );
    urlStr.replace( "{y}", QString::number( id.y() ) );
    urlStr.replace( "{west}", QString::number( bbox.west( GeoDataCoordinates::Degree ), 'f', 12 ) );
    urlStr.replace( "{south}", QString::number( bbox.south( GeoDataCoordinates::Degree ), 'f', 12 ) );
    urlStr.replace( "{east}", QString::number( bbox.east( GeoDataCoordinates::Degree ), 'f', 12 ) );
    urlStr.replace( "{north}", QString::number( bbox.north( GeoDataCoordinates::Degree ), 'f', 12 ) );

    return QUrl( urlStr );
}

QString CustomServerLayout::name() const
{
    return "Custom";
}


WmsServerLayout::WmsServerLayout( GeoSceneTileDataset *texture )
    : ServerLayout( texture )
{
}

QUrl WmsServerLayout::downloadUrl( const QUrl &prototypeUrl, const Marble::TileId &tileId ) const
{
    GeoDataLatLonBox box;
    m_textureLayer->tileProjection()->geoCoordinates(tileId, box);

    QUrlQuery url(prototypeUrl.query());
    url.addQueryItem( "service", "WMS" );
    url.addQueryItem( "request", "GetMap" );
    url.addQueryItem( "version", "1.1.1" );
    if ( !url.hasQueryItem( "styles" ) )
        url.addQueryItem( "styles", "" );
    if ( !url.hasQueryItem( "format" ) ) {
        if (m_textureLayer->fileFormat().toLower() == QLatin1String("jpg"))
            url.addQueryItem( "format", "image/jpeg" );
        else
            url.addQueryItem("format", QLatin1String("image/") + m_textureLayer->fileFormat().toLower());
    }
    if ( !url.hasQueryItem( "srs" ) ) {
        url.addQueryItem( "srs", epsgCode() );
    }
    if ( !url.hasQueryItem( "layers" ) )
        url.addQueryItem( "layers", m_textureLayer->name() );
    url.addQueryItem( "width", QString::number( m_textureLayer->tileSize().width() ) );
    url.addQueryItem( "height", QString::number( m_textureLayer->tileSize().height() ) );
    url.addQueryItem( "bbox", QString( "%1,%2,%3,%4" ).arg( QString::number( box.west( GeoDataCoordinates::Degree ), 'f', 12 ) )
                                                      .arg( QString::number( box.south( GeoDataCoordinates::Degree ), 'f', 12 ) )
                                                      .arg( QString::number( box.east( GeoDataCoordinates::Degree ), 'f', 12 ) )
                                                      .arg( QString::number( box.north( GeoDataCoordinates::Degree ), 'f', 12 ) ) );
    QUrl finalUrl = prototypeUrl;
    finalUrl.setQuery(url);
    return finalUrl;
}

QString WmsServerLayout::name() const
{
    return "WebMapService";
}

QString WmsServerLayout::epsgCode() const
{
    switch (m_textureLayer->tileProjectionType()) {
        case GeoSceneAbstractTileProjection::Equirectangular:
            return "EPSG:4326";
        case GeoSceneAbstractTileProjection::Mercator:
            return "EPSG:3785";
    }

    Q_ASSERT( false ); // not reached
    return QString();
}

QuadTreeServerLayout::QuadTreeServerLayout( GeoSceneTileDataset *textureLayer )
    : ServerLayout( textureLayer )
{
}

QUrl QuadTreeServerLayout::downloadUrl( const QUrl &prototypeUrl, const Marble::TileId &id ) const
{
    QString urlStr = prototypeUrl.toString( QUrl::DecodeReserved );

    urlStr.replace( "{quadIndex}", encodeQuadTree( id ) );

    return QUrl( urlStr );
}

QString QuadTreeServerLayout::name() const
{
    return "QuadTree";
}

QString QuadTreeServerLayout::encodeQuadTree( const Marble::TileId &id )
{
    QString tileNum;

    for ( int i = id.zoomLevel(); i >= 0; i-- ) {
        const int tileX = (id.x() >> i) % 2;
        const int tileY = (id.y() >> i) % 2;
        const int num = ( 2 * tileY ) + tileX;

        tileNum += QString::number( num );
    }

    return tileNum;
}

TmsServerLayout::TmsServerLayout(GeoSceneTileDataset *textureLayer )
    : ServerLayout( textureLayer )
{
}

QUrl TmsServerLayout::downloadUrl( const QUrl &prototypeUrl, const TileId &id ) const
{
    const QString suffix = m_textureLayer->fileFormat().toLower();
    // y coordinate in TMS start at the bottom of the map (South) and go upwards,
    // opposed to OSM which start at the top.
    //
    // http://wiki.osgeo.org/wiki/Tile_Map_Service_Specification
    int y_frombottom = ( 1<<id.zoomLevel() ) - id.y() - 1 ;

    const QString path = QString( "%1/%2/%3.%4" ).arg( id.zoomLevel() )
                                                 .arg( id.x() )
                                                 .arg( y_frombottom )
                                                 .arg( suffix );
    QUrl url = prototypeUrl;
    url.setPath( url.path() + path );

    return url;
}

QString TmsServerLayout::name() const
{
    return "TileMapService";
}

}
