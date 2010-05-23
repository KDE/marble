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

QUrl LatLonBoxServerLayout::downloadUrl( const QUrl &prototypeUrl, const Marble::TileId &tileId ) const
{
    const qint64 radius = ( 1 << ( tileId.zoomLevel() - 1 ) );
    const qint64 x = tileId.x();
    const qint64 y = tileId.y();

    const qreal latBottom = atan( sinh( ( radius - y - 1 ) / (double)radius * M_PI ) ) * 180.0 / M_PI;
    const qreal latTop    = atan( sinh( ( radius - y     ) / (double)radius * M_PI ) ) * 180.0 / M_PI;
    const qreal lonLeft   =             ( x - radius     ) / (double)radius            * 180.0;
    const qreal lonRight  =             ( x - radius + 1 ) / (double)radius            * 180.0;

    QString strUrl = prototypeUrl.toString();

    strUrl.replace( "{lonLeft}",   QString::number( lonLeft   ) );
    strUrl.replace( "{latBottom}", QString::number( latBottom ) );
    strUrl.replace( "{lonRight}",  QString::number( lonRight  ) );
    strUrl.replace( "{latTop}",    QString::number( latTop    ) );

    return QUrl::fromEncoded( strUrl.toLatin1() );
}

}
