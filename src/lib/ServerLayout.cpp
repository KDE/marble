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
#include "global.h"
#include "TileId.h"

#include <math.h>

namespace Marble
{

ServerLayout::~ServerLayout()
{
}


MarbleServerLayout::MarbleServerLayout( GeoSceneTexture *textureLayer )
    : m_textureLayer( textureLayer )
{
}

QUrl MarbleServerLayout::downloadUrl( const QUrl &prototypeUrl, const TileId &id ) const
{
    QUrl url = prototypeUrl;
    url.setPath( url.path() + m_textureLayer->relativeTileFileName( id ) );

    return url;
}


OsmServerLayout::OsmServerLayout( GeoSceneTexture *textureLayer )
    : m_textureLayer( textureLayer )
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


QUrl CustomServerLayout::downloadUrl( const QUrl &prototypeUrl, const TileId &id ) const
{
    QString urlStr = prototypeUrl.toString();

    urlStr.replace( "{zoomLevel}", QString::number( id.zoomLevel() ) );
    urlStr.replace( "{x}", QString::number( id.x() ) );
    urlStr.replace( "{y}", QString::number( id.y() ) );

    return QUrl( urlStr );
}

WmsServerLayout::WmsServerLayout( GeoSceneTexture *texture )
    : m_textureLayer( texture )
{
}

QUrl WmsServerLayout::downloadUrl( const QUrl &prototypeUrl, const Marble::TileId &tileId ) const
{
    const qint64 radius = ( 1 << ( tileId.zoomLevel() - 1 ) );
    const qint64 x = tileId.x();
    const qint64 y = tileId.y();

    const qreal latBottom = ( radius - y - 1 ) / (double)radius *  90.0;
    const qreal latTop    = ( radius - y     ) / (double)radius *  90.0;
    const qreal lonLeft   = ( x - radius     ) / (double)radius * 180.0;
    const qreal lonRight  = ( x - radius + 1 ) / (double)radius * 180.0;

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
        switch ( m_textureLayer->projection() ) {
            case GeoSceneTexture::Equirectangular:
                url.addQueryItem( "srs", "EPSG:4326" );
                break;
            case GeoSceneTexture::Mercator:
                url.addQueryItem( "srs", "EPSG:3785" );
                break;
        }
    }
    if ( !url.hasQueryItem( "layers" ) )
        url.addQueryItem( "layers", m_textureLayer->name() );
    url.addQueryItem( "width", QString::number( m_textureLayer->tileSize().width() ) );
    url.addQueryItem( "height", QString::number( m_textureLayer->tileSize().height() ) );
    url.addQueryItem( "bbox", QString( "%1,%2,%3,%4" ).arg( QString::number( lonLeft, 'f', 12 ) )
                                                      .arg( QString::number( latBottom, 'f', 12 ) )
                                                      .arg( QString::number( lonRight, 'f', 12 ) )
                                                      .arg( QString::number( latTop, 'f', 12 ) ) );

    return url;
}

}
