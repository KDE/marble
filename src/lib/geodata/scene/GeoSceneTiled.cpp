/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright (C) 2008 Torsten Rahn <rahn@kde.org>

 Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#include "GeoSceneTypes.h"
#include "GeoSceneTiled.h"

#include "DownloadPolicy.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "ServerLayout.h"
#include "TileId.h"

#include <QtGui/QImage>

namespace Marble
{

GeoSceneTiled::GeoSceneTiled( const QString& name )
    : GeoSceneAbstractDataset( name ),
      m_sourceDir(),
      m_installMap(),
      m_storageLayoutMode(Marble),
      m_serverLayout( new MarbleServerLayout( this ) ),
      m_levelZeroColumns( defaultLevelZeroColumns ),
      m_levelZeroRows( defaultLevelZeroRows ),
      m_maximumTileLevel( -1 ),
      m_projection( Equirectangular ),
      m_blending(),
      m_downloadUrls(),
      m_nextUrl( m_downloadUrls.constEnd() )
{
}

GeoSceneTiled::~GeoSceneTiled()
{
    qDeleteAll( m_downloadPolicies );
    delete m_serverLayout;
}

const char* GeoSceneTiled::nodeType() const
{
    return GeoSceneTypes::GeoSceneTiledType;
}

QString GeoSceneTiled::sourceDir() const
{
    return m_sourceDir;
}

void GeoSceneTiled::setSourceDir( const QString& sourceDir )
{
    m_sourceDir = sourceDir;
}

QString GeoSceneTiled::installMap() const
{
    return m_installMap;
}

void GeoSceneTiled::setInstallMap( const QString& installMap )
{
    m_installMap = installMap;
}

GeoSceneTiled::StorageLayout GeoSceneTiled::storageLayout() const
{
    return m_storageLayoutMode;
}

void GeoSceneTiled::setStorageLayout( const StorageLayout layout )
{
    m_storageLayoutMode = layout;
}

void GeoSceneTiled::setServerLayout( const ServerLayout *layout )
{
    delete m_serverLayout;
    m_serverLayout = layout;
}

const ServerLayout* GeoSceneTiled::serverLayout() const
{
    return m_serverLayout;
}

int GeoSceneTiled::levelZeroColumns() const
{
   return m_levelZeroColumns;
}

void GeoSceneTiled::setLevelZeroColumns( const int columns )
{
    m_levelZeroColumns = columns;
}

int GeoSceneTiled::levelZeroRows() const
{
    return m_levelZeroRows;
}

void GeoSceneTiled::setLevelZeroRows( const int rows )
{
    m_levelZeroRows = rows;
}

int GeoSceneTiled::maximumTileLevel() const
{
    return m_maximumTileLevel;
}

void GeoSceneTiled::setMaximumTileLevel( const int maximumTileLevel )
{
    m_maximumTileLevel = maximumTileLevel;
}

QVector<QUrl> GeoSceneTiled::downloadUrls() const
{
    return m_downloadUrls;
}

const QSize GeoSceneTiled::tileSize() const
{
    if ( m_tileSize.isEmpty() ) {
        const TileId id( 0, 0, 0, 0 );
        QString const fileName = relativeTileFileName( id );
        QFileInfo const dirInfo( fileName );
        QString const path = dirInfo.isAbsolute() ? fileName : MarbleDirs::path( fileName );

        QImage testTile( path );

        if ( testTile.isNull() ) {
            mDebug() << "Tile size is missing in dgml and no base tile found in " << themeStr();
            mDebug() << "Using default tile size " << c_defaultTileSize;
            m_tileSize = QSize( c_defaultTileSize, c_defaultTileSize );
        } else {
            m_tileSize = testTile.size();
        }

        if ( m_tileSize.isEmpty() ) {
            mDebug() << "Tile width or height cannot be 0. Falling back to default tile size.";
            m_tileSize = QSize( c_defaultTileSize, c_defaultTileSize );
        }
    }

    Q_ASSERT( !m_tileSize.isEmpty() );
    return m_tileSize;
}

void GeoSceneTiled::setTileSize( const QSize &tileSize )
{
    if ( tileSize.isEmpty() ) {
        mDebug() << "Ignoring invalid tile size " << tileSize;
    } else {
        m_tileSize = tileSize;
    }
}

GeoSceneTiled::Projection GeoSceneTiled::projection() const
{
    return m_projection;
}

void GeoSceneTiled::setProjection( const Projection projection )
{
    m_projection = projection;
}

// Even though this method changes the internal state, it may be const
// because the compiler is forced to invoke this method for different TileIds.
QUrl GeoSceneTiled::downloadUrl( const TileId &id ) const
{
    // default download url
    if ( m_downloadUrls.empty() )
        return m_serverLayout->downloadUrl( QUrl( "http://files.kde.org/marble/" ), id );

    if ( m_nextUrl == m_downloadUrls.constEnd() )
        m_nextUrl = m_downloadUrls.constBegin();

    const QUrl url = m_serverLayout->downloadUrl( *m_nextUrl, id );

    ++m_nextUrl;

    return url;
}

void GeoSceneTiled::addDownloadUrl( const QUrl & url )
{
    m_downloadUrls.append( url );
    // FIXME: this could be done only once
    m_nextUrl = m_downloadUrls.constBegin();
}

QString GeoSceneTiled::relativeTileFileName( const TileId &id ) const
{
    const QString suffix = fileFormat().toLower();

    QString relFileName;

    switch ( m_storageLayoutMode ) {
    default:
        mDebug() << Q_FUNC_INFO << "Invalid storage layout mode! Falling back to default.";
    case GeoSceneTiled::Marble:
        relFileName = QString( "%1/%2/%3/%3_%4.%5" )
            .arg( themeStr() )
            .arg( id.zoomLevel() )
            .arg( id.y(), tileDigits, 10, QChar('0') )
            .arg( id.x(), tileDigits, 10, QChar('0') )
            .arg( suffix );
        break;
    case GeoSceneTiled::OpenStreetMap:
        relFileName = QString( "%1/%2/%3/%4.%5" )
            .arg( themeStr() )
            .arg( id.zoomLevel() )
            .arg( id.x() )
            .arg( id.y() )
            .arg( suffix );
        break;
    case GeoSceneTiled::TileMapService:
        relFileName = QString( "%1/%2/%3/%4.%5" )
            .arg( themeStr() )
            .arg( id.zoomLevel() )
            .arg( id.x() )
            .arg( ( 1<<id.zoomLevel() ) - id.y() - 1 )  //Y coord in TMS runs from bottom to top
            .arg( suffix );
        break;
    }

    return relFileName;
}

QString GeoSceneTiled::themeStr() const
{
    QFileInfo const dirInfo( sourceDir() );
    return dirInfo.isAbsolute() ? sourceDir() : "maps/" + sourceDir();
}

QList<const DownloadPolicy *> GeoSceneTiled::downloadPolicies() const
{
    return m_downloadPolicies;
}

void GeoSceneTiled::addDownloadPolicy( const DownloadUsage usage, const int maximumConnections )
{
    DownloadPolicy * const policy = new DownloadPolicy( DownloadPolicyKey( hostNames(), usage ));
    policy->setMaximumConnections( maximumConnections );
    m_downloadPolicies.append( policy );
    mDebug() << "added download policy" << hostNames() << usage << maximumConnections;
}

QString GeoSceneTiled::type()
{
    return "tiled";
}

QStringList GeoSceneTiled::hostNames() const
{
    QStringList result;
    QVector<QUrl>::const_iterator pos = m_downloadUrls.constBegin();
    QVector<QUrl>::const_iterator const end = m_downloadUrls.constEnd();
    for (; pos != end; ++pos )
        result.append( (*pos).host() );
    return result;
}

}
