/*
    Copyright (C) 2008 Torsten Rahn <rahn@kde.org>
    Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

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
    return "GeoSceneTiled";
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
        const TileId id( sourceDir(), 0, 0, 0 );
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
    }

    return relFileName;
}

QString GeoSceneTiled::themeStr() const
{
    QFileInfo const dirInfo( sourceDir() );
    return dirInfo.isAbsolute() ? sourceDir() : "maps/" + sourceDir();
}

QList<DownloadPolicy *> GeoSceneTiled::downloadPolicies() const
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
    return "texture";
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
