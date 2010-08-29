//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

// Own
#include "ServerLayout.h"

#include "GeoSceneTexture.h"
#include "EquirectProjection.h"
#include "MercatorProjection.h"
#include "global.h"
#include "TileId.h"

#include <math.h>

namespace Marble
{

ServerLayout::ServerLayout( GeoSceneTexture *textureLayer )
    : m_textureLayer( textureLayer )
{
}

ServerLayout::~ServerLayout()
{
}

qint64 ServerLayout::numTilesX( const Marble::TileId& tileId ) const
{
    return ( 1 << tileId.zoomLevel() ) * m_textureLayer->levelZeroColumns();
}

qint64 ServerLayout::numTilesY( const Marble::TileId& tileId ) const
{
    return ( 1 << tileId.zoomLevel() ) * m_textureLayer->levelZeroRows();
}

MarbleServerLayout::MarbleServerLayout( GeoSceneTexture *textureLayer )
    : ServerLayout( textureLayer )
{
}

QUrl MarbleServerLayout::downloadUrl( const QUrl &prototypeUrl, const TileId &id ) const
{
    QUrl url = prototypeUrl;
    url.setPath( url.path() + m_textureLayer->relativeTileFileName( id ) );

    return url;
}


OsmServerLayout::OsmServerLayout( GeoSceneTexture *textureLayer )
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


CustomServerLayout::CustomServerLayout( GeoSceneTexture *texture )
    : ServerLayout( texture )
{
}

QUrl CustomServerLayout::downloadUrl( const QUrl &prototypeUrl, const TileId &id ) const
{
    QString urlStr = prototypeUrl.toString();

    urlStr.replace( "{zoomLevel}", QString::number( id.zoomLevel() ) );
    urlStr.replace( "{x}", QString::number( id.x() ) );
    urlStr.replace( "{y}", QString::number( id.y() ) );

    return QUrl( urlStr );
}

WmsServerLayout::WmsServerLayout( GeoSceneTexture *texture )
    : ServerLayout( texture )
{
}

QUrl WmsServerLayout::downloadUrl( const QUrl &prototypeUrl, const Marble::TileId &tileId ) const
{
    const QPointF bottomLeft = this->bottomLeft( tileId );
    const QPointF topRight = this->topRight( tileId );

    QUrl url = prototypeUrl;
    url.addQueryItem( "service", "WMS" );
    url.addQueryItem( "request", "GetMap" );
    url.addQueryItem( "version", "1.1.1" );
    if ( !url.hasQueryItem( "styles" ) )
        url.addQueryItem( "styles", "" );
    if ( !url.hasQueryItem( "format" ) ) {
        if ( m_textureLayer->fileFormat().toLower() == "jpg" )
            url.addQueryItem( "format", "image/jpeg" );
        else
            url.addQueryItem( "format", "image/" + m_textureLayer->fileFormat().toLower() );
    }
    if ( !url.hasQueryItem( "srs" ) ) {
        url.addQueryItem( "srs", epsgCode() );
    }
    if ( !url.hasQueryItem( "layers" ) )
        url.addQueryItem( "layers", m_textureLayer->name() );
    url.addQueryItem( "width", QString::number( m_textureLayer->tileSize().width() ) );
    url.addQueryItem( "height", QString::number( m_textureLayer->tileSize().height() ) );
    url.addQueryItem( "bbox", QString( "%1,%2,%3,%4" ).arg( QString::number( bottomLeft.x(), 'f', 14 ) )
                                                      .arg( QString::number( bottomLeft.y(), 'f', 14 ) )
                                                      .arg( QString::number( topRight.x(), 'f', 14 ) )
                                                      .arg( QString::number( topRight.y(), 'f', 14 ) ) );

    return url;
}

QPointF WmsServerLayout::bottomLeft( const Marble::TileId &tileId ) const
{
    static const EquirectProjection equirectangular;
    static const MercatorProjection mercator;

    const AbstractProjection *projection = 0;

    switch( m_textureLayer->projection() )
    {
    case GeoSceneTexture::Equirectangular:
        projection = &equirectangular;
        break;
    case GeoSceneTexture::Mercator:
        projection = &mercator;
        break;
    }

    Q_ASSERT( projection != 0 );

    const qreal x = ( tileId.x()     ) / (qreal)numTilesX( tileId );
    const qreal y = ( tileId.y() + 1 ) / (qreal)numTilesY( tileId );

    qreal lon;
    qreal lat;
    projection->geoCoordinates( x, y, lon, lat, GeoDataCoordinates::Degree );

    return QPointF( lon, lat );
}

QPointF WmsServerLayout::topRight( const Marble::TileId &tileId ) const
{
    static const EquirectProjection equirectangular;
    static const MercatorProjection mercator;

    const AbstractProjection *projection = 0;

    switch( m_textureLayer->projection() )
    {
    case GeoSceneTexture::Equirectangular:
        projection = &equirectangular;
        break;
    case GeoSceneTexture::Mercator:
        projection = &mercator;
        break;
    }

    Q_ASSERT( projection != 0 );

    const qreal x = ( tileId.x() + 1  ) / (qreal)numTilesX( tileId );
    const qreal y = ( tileId.y()      ) / (qreal)numTilesY( tileId );

    qreal lon;
    qreal lat;
    projection->geoCoordinates( x, y, lon, lat, GeoDataCoordinates::Degree );

    return QPointF( lon, lat );
}

QString WmsServerLayout::epsgCode() const
{
    switch ( m_textureLayer->projection() ) {
        case GeoSceneTexture::Equirectangular:
            return "EPSG:4326";
        case GeoSceneTexture::Mercator:
            return "EPSG:3785";
    }

    Q_ASSERT( false ); // not reached
    return QString();
}

}
