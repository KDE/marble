/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright (C) 2008 Torsten Rahn <rahn@kde.org>

 Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#include "GeoSceneTileDataset.h"
#include "GeoSceneTypes.h"
#include "GeoSceneEquirectTileProjection.h"
#include "GeoSceneMercatorTileProjection.h"

#include "DownloadPolicy.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "ServerLayout.h"
#include "TileId.h"

#include <QImage>
#include <QUrl>

namespace Marble
{

GeoSceneTileDataset::GeoSceneTileDataset( const QString& name )
    : GeoSceneAbstractDataset( name ),
      m_sourceDir(),
      m_installMap(),
      m_storageLayoutMode(Marble),
      m_serverLayout( new MarbleServerLayout( this ) ),
      m_levelZeroColumns( defaultLevelZeroColumns ),
      m_levelZeroRows( defaultLevelZeroRows ),
      m_minimumTileLevel(0),
      m_maximumTileLevel( -1 ),
      m_tileProjection(new GeoSceneEquirectTileProjection()),
      m_blending(),
      m_downloadUrls(),
      m_nextUrl( m_downloadUrls.constEnd() )
{
    m_tileProjection->setLevelZeroColumns(m_levelZeroColumns);
    m_tileProjection->setLevelZeroRows(m_levelZeroRows);
}

GeoSceneTileDataset::~GeoSceneTileDataset()
{
    qDeleteAll( m_downloadPolicies );
    delete m_serverLayout;
    delete m_tileProjection;
}

const char* GeoSceneTileDataset::nodeType() const
{
    return GeoSceneTypes::GeoSceneTileDatasetType;
}

QString GeoSceneTileDataset::sourceDir() const
{
    return m_sourceDir;
}

void GeoSceneTileDataset::setSourceDir( const QString& sourceDir )
{
    m_sourceDir = sourceDir;
}

QString GeoSceneTileDataset::installMap() const
{
    return m_installMap;
}

void GeoSceneTileDataset::setInstallMap( const QString& installMap )
{
    m_installMap = installMap;
}

GeoSceneTileDataset::StorageLayout GeoSceneTileDataset::storageLayout() const
{
    return m_storageLayoutMode;
}

void GeoSceneTileDataset::setStorageLayout( const StorageLayout layout )
{
    m_storageLayoutMode = layout;
}

void GeoSceneTileDataset::setServerLayout( const ServerLayout *layout )
{
    delete m_serverLayout;
    m_serverLayout = layout;
}

const ServerLayout* GeoSceneTileDataset::serverLayout() const
{
    return m_serverLayout;
}

int GeoSceneTileDataset::levelZeroColumns() const
{
   return m_levelZeroColumns;
}

void GeoSceneTileDataset::setLevelZeroColumns( const int columns )
{
    m_levelZeroColumns = columns;
    m_tileProjection->setLevelZeroColumns(m_levelZeroColumns);
}

int GeoSceneTileDataset::levelZeroRows() const
{
    return m_levelZeroRows;
}

void GeoSceneTileDataset::setLevelZeroRows( const int rows )
{
    m_levelZeroRows = rows;
    m_tileProjection->setLevelZeroRows(m_levelZeroRows);
}

int GeoSceneTileDataset::maximumTileLevel() const
{
    return m_maximumTileLevel;
}

void GeoSceneTileDataset::setMaximumTileLevel( const int maximumTileLevel )
{
    m_maximumTileLevel = maximumTileLevel;
}

int GeoSceneTileDataset::minimumTileLevel() const
{
    return m_minimumTileLevel;
}

void GeoSceneTileDataset::setMinimumTileLevel(int level)
{
    m_minimumTileLevel = level;
}

void GeoSceneTileDataset::setTileLevels(const QString &tileLevels)
{
    if (tileLevels.isEmpty()) {
        m_tileLevels.clear();
        return;
    }

    const QStringList values = tileLevels.split(QLatin1Char(','));
    foreach(const QString &value, values) {
        bool canParse(false);
        int const tileLevel = value.trimmed().toInt(&canParse);
        if (canParse && tileLevel >= 0 && tileLevel < 100) {
            m_tileLevels << tileLevel;
        } else {
            mDebug() << "Cannot parse tile level part " << value << " in " << tileLevels << ", ignoring it.";
        }
    }

    if (!m_tileLevels.isEmpty()) {
        qSort(m_tileLevels);
        m_minimumTileLevel = m_tileLevels.first();
        m_maximumTileLevel = m_tileLevels.last();
    }
}

QVector<int> GeoSceneTileDataset::tileLevels() const
{
    return m_tileLevels;
}

QVector<QUrl> GeoSceneTileDataset::downloadUrls() const
{
    return m_downloadUrls;
}

const QSize GeoSceneTileDataset::tileSize() const
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

void GeoSceneTileDataset::setTileSize( const QSize &tileSize )
{
    if ( tileSize.isEmpty() ) {
        mDebug() << "Ignoring invalid tile size " << tileSize;
    } else {
        m_tileSize = tileSize;
    }
}

void GeoSceneTileDataset::setTileProjection(GeoSceneAbstractTileProjection::Type projectionType)
{
    if (m_tileProjection->type() == projectionType) {
        return;
    }

    delete m_tileProjection;
    if (projectionType == GeoSceneAbstractTileProjection::Mercator) {
        m_tileProjection = new GeoSceneMercatorTileProjection();
    } else {
        m_tileProjection = new GeoSceneEquirectTileProjection();
    }

    m_tileProjection->setLevelZeroColumns(m_levelZeroColumns);
    m_tileProjection->setLevelZeroRows(m_levelZeroRows);
}

const GeoSceneAbstractTileProjection * GeoSceneTileDataset::tileProjection() const
{
    return m_tileProjection;
}

GeoSceneAbstractTileProjection::Type GeoSceneTileDataset::tileProjectionType() const
{
    return m_tileProjection->type();
}

// Even though this method changes the internal state, it may be const
// because the compiler is forced to invoke this method for different TileIds.
QUrl GeoSceneTileDataset::downloadUrl( const TileId &id ) const
{
    // default download url
    if ( m_downloadUrls.empty() ) {
        QUrl const defaultUrl = QUrl(QLatin1String("https://maps.kde.org/") + m_serverLayout->sourceDir());
        mDebug() << "No download URL specified for tiles stored in "
                 << m_sourceDir << ", falling back to " << defaultUrl.toString();
        return m_serverLayout->downloadUrl(defaultUrl, id);
    }

    if ( m_nextUrl == m_downloadUrls.constEnd() )
        m_nextUrl = m_downloadUrls.constBegin();

    const QUrl url = m_serverLayout->downloadUrl( *m_nextUrl, id );

    ++m_nextUrl;

    return url;
}

void GeoSceneTileDataset::addDownloadUrl( const QUrl & url )
{
    m_downloadUrls.append( url );
    // FIXME: this could be done only once
    m_nextUrl = m_downloadUrls.constBegin();
}

QString GeoSceneTileDataset::relativeTileFileName( const TileId &id ) const
{
    const QString suffix = fileFormat().toLower();

    QString relFileName;

    switch ( m_storageLayoutMode ) {
    default:
        mDebug() << Q_FUNC_INFO << "Invalid storage layout mode! Falling back to default.";
    case GeoSceneTileDataset::Marble:
        relFileName = QString( "%1/%2/%3/%3_%4.%5" )
            .arg( themeStr() )
            .arg( id.zoomLevel() )
            .arg(id.y(), tileDigits, 10, QLatin1Char('0'))
            .arg(id.x(), tileDigits, 10, QLatin1Char('0'))
            .arg( suffix );
        break;
    case GeoSceneTileDataset::OpenStreetMap:
        relFileName = QString( "%1/%2/%3/%4.%5" )
            .arg( themeStr() )
            .arg( id.zoomLevel() )
            .arg( id.x() )
            .arg( id.y() )
            .arg( suffix );
        break;
    case GeoSceneTileDataset::TileMapService:
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

QString GeoSceneTileDataset::themeStr() const
{
    QFileInfo const dirInfo( sourceDir() );
    return dirInfo.isAbsolute() ? sourceDir() : QLatin1String("maps/") + sourceDir();
}

QList<const DownloadPolicy *> GeoSceneTileDataset::downloadPolicies() const
{
    return m_downloadPolicies;
}

void GeoSceneTileDataset::addDownloadPolicy( const DownloadUsage usage, const int maximumConnections )
{
    DownloadPolicy * const policy = new DownloadPolicy( DownloadPolicyKey( hostNames(), usage ));
    policy->setMaximumConnections( maximumConnections );
    m_downloadPolicies.append( policy );
    mDebug() << "added download policy" << hostNames() << usage << maximumConnections;
}

QStringList GeoSceneTileDataset::hostNames() const
{
    QStringList result;
    result.reserve(m_downloadUrls.size());

    QVector<QUrl>::const_iterator pos = m_downloadUrls.constBegin();
    QVector<QUrl>::const_iterator const end = m_downloadUrls.constEnd();
    for (; pos != end; ++pos )
        result.append( (*pos).host() );
    return result;
}

}
