//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008, 2009, 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
// Copyright 2008-2009      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2010-2011 Bernhard Beschow  <bbeschow@cs.tu-berlin.de>
//

#include "MarbleModel.h"

#include <cmath>

#include <QtCore/QAtomicInt>
#include <QtCore/QPointer>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QSet>
#include <QtGui/QItemSelectionModel>
#include <QtGui/QSortFilterProxyModel>

#if (QT_VERSION >= 0x040700 && QT_VERSION < 0x040800)
// See comment below why this is needed
#include <QtNetwork/QNetworkConfigurationManager>
#endif

#include "kdescendantsproxymodel.h"

#include "MapThemeManager.h"
#include "MarbleGlobal.h"
#include "MarbleDebug.h"

#include "GeoSceneDocument.h"
#include "GeoSceneFilter.h"
#include "GeoSceneHead.h"
#include "GeoSceneLayer.h"
#include "GeoSceneMap.h"
#include "GeoScenePalette.h"
#include "GeoSceneTiled.h"
#include "GeoSceneVector.h"
#include "GeoSceneXmlDataSource.h"

#include "GeoDataDocument.h"
#include "GeoDataStyle.h"
#include "GeoDataTypes.h"

#include "DgmlAuxillaryDictionary.h"
#include "MarbleClock.h"
#include "FileStoragePolicy.h"
#include "FileStorageWatcher.h"
#include "FileViewModel.h"
#include "PositionTracking.h"
#include "HttpDownloadManager.h"
#include "MarbleDirs.h"
#include "FileManager.h"
#include "GeoDataTreeModel.h"
#include "Planet.h"
#include "PluginManager.h"
#include "StoragePolicy.h"
#include "SunLocator.h"
#include "TileCreator.h"
#include "TileCreatorDialog.h"
#include "TileLoader.h"
#include "routing/RoutingManager.h"
#include "BookmarkManager.h"
#include "ElevationModel.h"

namespace Marble
{

class MarbleModelPrivate
{
 public:
    MarbleModelPrivate()
        : m_clock(),
          m_planet( new Planet( "earth" ) ),
          m_sunLocator( &m_clock, m_planet ),
          m_pluginManager(),
          m_mapThemeManager(),
          m_homePoint( -9.4, 54.8, 0.0, GeoDataCoordinates::Degree ),  // Some point that tackat defined. :-)
          m_homeZoom( 1050 ),
          m_mapTheme( 0 ),
          m_storagePolicy( MarbleDirs::localPath() ),
          m_downloadManager( &m_storagePolicy, &m_pluginManager ),
          m_storageWatcher( MarbleDirs::localPath() ),
          m_fileManager( 0 ),
          m_fileviewmodel(),
          m_treemodel(),
          m_descendantproxy(),
          m_sortproxy(),
          m_placemarkselectionmodel( 0 ),
          m_positionTracking( &m_treemodel ),
          m_trackedPlacemark( 0 ),
          m_bookmarkManager( &m_treemodel ),
          m_routingManager( 0 ),
          m_legend( 0 ),
          m_workOffline( false )
    {
        m_sortproxy.setFilterFixedString( GeoDataTypes::GeoDataPlacemarkType );
        m_sortproxy.setFilterKeyColumn( 1 );
        m_sortproxy.setSourceModel( &m_descendantproxy );
        // FIXME ANDER TEST TO SEE IF GEODATADOCUMENTS CACHE WORKS
        m_descendantproxy.setSourceModel( &m_treemodel );
    }

    ~MarbleModelPrivate()
    {
    }

    // Misc stuff.
    MarbleClock              m_clock;
    Planet                  *m_planet;
    SunLocator               m_sunLocator;

    PluginManager            m_pluginManager;
    MapThemeManager          m_mapThemeManager;

    // The home position
    GeoDataCoordinates       m_homePoint;
    int                      m_homeZoom;

    // View and paint stuff
    GeoSceneDocument        *m_mapTheme;

    FileStoragePolicy        m_storagePolicy;
    HttpDownloadManager      m_downloadManager;

    // Cache related
    FileStorageWatcher       m_storageWatcher;

    // Places on the map
    FileManager             *m_fileManager;

    FileViewModel            m_fileviewmodel;
    GeoDataTreeModel         m_treemodel;
    KDescendantsProxyModel   m_descendantproxy;
    QSortFilterProxyModel    m_sortproxy;

    // Selection handling
    QItemSelectionModel      m_placemarkselectionmodel;

    //Gps Stuff
    PositionTracking         m_positionTracking;

    const GeoDataPlacemark  *m_trackedPlacemark;

    BookmarkManager          m_bookmarkManager;
    RoutingManager          *m_routingManager;
    QTextDocument           *m_legend;

    bool                     m_workOffline;

    ElevationModel           *m_elevationModel;
};

MarbleModel::MarbleModel( QObject *parent )
    : QObject( parent ),
      d( new MarbleModelPrivate() )
{
#if (QT_VERSION >= 0x040700 && QT_VERSION < 0x040800)
    // fix for KDE bug 288612
    // Due to a race condition in Qt 4.7 (QTBUG-22107), a segfault might occur at
    // startup when e.g. reverse geocoding is called very early.
    // The race condition can be avoided by instantiating QNetworkConfigurationManager
    // when only one thread is running (i.e. here).
    // QNetworkConfigurationManager was introduced in Qt 4.7, the bug is fixed
    // in 4.8, thus the Qt version check.
    new QNetworkConfigurationManager( this );
#endif

    // The thread will be started at setting persistent tile cache size.
    connect( this, SIGNAL( themeChanged( QString ) ),
             &d->m_storageWatcher, SLOT( updateTheme( QString ) ) );

    // connect the StoragePolicy used by the download manager to the FileStorageWatcher
    connect( &d->m_storagePolicy, SIGNAL( cleared() ),
             &d->m_storageWatcher, SLOT( resetCurrentSize() ) );
    connect( &d->m_storagePolicy, SIGNAL( sizeChanged( qint64 ) ),
             &d->m_storageWatcher, SLOT( addToCurrentSize( qint64 ) ) );

    d->m_fileManager = new FileManager( this );
    d->m_fileviewmodel.setFileManager( d->m_fileManager );
    connect( d->m_fileManager,    SIGNAL( fileAdded(int)),
             &d->m_fileviewmodel, SLOT( append(int)) );
    connect( d->m_fileManager,    SIGNAL( fileRemoved(int)),
             &d->m_fileviewmodel, SLOT(remove(int)) );

    d->m_routingManager = new RoutingManager( this, this );

    connect(&d->m_clock,   SIGNAL( timeChanged() ),
            &d->m_sunLocator, SLOT( update() ) );

    d->m_elevationModel = new ElevationModel( this );

}

MarbleModel::~MarbleModel()
{
    delete d->m_fileManager;
    delete d->m_mapTheme;
    delete d->m_planet;
    delete d;

    mDebug() << "Model deleted:" << this;
}

BookmarkManager *MarbleModel::bookmarkManager()
{
    return &d->m_bookmarkManager;
}

QString MarbleModel::mapThemeId() const
{
    QString mapThemeId = "";

    if (d->m_mapTheme)
        mapThemeId = d->m_mapTheme->head()->mapThemeId();

    return mapThemeId;
}

GeoSceneDocument *MarbleModel::mapTheme()
{
    return d->m_mapTheme;
}

const GeoSceneDocument *MarbleModel::mapTheme() const
{
    return d->m_mapTheme;
}

// Set a particular theme for the map and load the appropriate tile level.
// If the tiles (for the lowest tile level) haven't been created already
// then create them here and now.
//
// FIXME: Move the tile creation dialogs out of this function.  Change
//        them into signals instead.
// FIXME: Get rid of 'currentProjection' here.  It's totally misplaced.
//

void MarbleModel::setMapThemeId( const QString &mapThemeId )
{
    if ( !mapThemeId.isEmpty() && mapThemeId == this->mapThemeId() )
        return;

    GeoSceneDocument *mapTheme = d->m_mapThemeManager.loadMapTheme( mapThemeId );
    if ( !mapTheme ) {
        // Check whether the previous theme works
        if ( d->m_mapTheme ){
            qWarning() << "Selected theme doesn't work, so we stick to the previous one";
            return;
        }

        // Fall back to default theme
        QString defaultTheme = "earth/srtm/srtm.dgml";
        qWarning() << "Falling back to default theme:" << defaultTheme;
        mapTheme = d->m_mapThemeManager.loadMapTheme( defaultTheme );
    }

    // If this last resort doesn't work either shed a tear and exit
    if ( !mapTheme ) {
        qWarning() << "Couldn't find a valid DGML map.";
        return;
    }

    // find the list of previous theme's geodata
    QStringList loadedContainers;
    if ( d->m_mapTheme ) {
        foreach ( GeoSceneLayer *layer, d->m_mapTheme->map()->layers() ) {
            if ( layer->backend() != dgml::dgmlValue_geodata )
                continue;

            if ( layer->datasets().count() <= 0 )
                continue;

            // look for documents
            foreach ( GeoSceneAbstractDataset *dataset, layer->datasets() ) {
                if( dataset->fileFormat() == "KML" ) {
                    QString containername = reinterpret_cast<GeoSceneXmlDataSource*>( dataset )->filename();
                    loadedContainers <<  containername;
                }
            }
        }
    }

    delete d->m_mapTheme;
    d->m_mapTheme = mapTheme;

    addDownloadPolicies( d->m_mapTheme );

    // Some output to show how to use this stuff ...
    mDebug() << "DGML2 Name       : " << d->m_mapTheme->head()->name();
/*
    mDebug() << "DGML2 Description: " << d->m_mapTheme->head()->description();

    if ( d->m_mapTheme->map()->hasTextureLayers() )
        mDebug() << "Contains texture layers! ";
    else
        mDebug() << "Does not contain any texture layers! ";

    mDebug() << "Number of SRTM textures: " << d->m_mapTheme->map()->layer("srtm")->datasets().count();

    if ( d->m_mapTheme->map()->hasVectorLayers() )
        mDebug() << "Contains vector layers! ";
    else
        mDebug() << "Does not contain any vector layers! ";
*/
    //Don't change the planet unless we have to...
    if( d->m_mapTheme->head()->target().toLower() != d->m_planet->id() ) {
        mDebug() << "Changing Planet";
        *(d->m_planet) = Planet( d->m_mapTheme->head()->target().toLower() );
        sunLocator()->setPlanet(d->m_planet);
    }

    QStringList loadList;
    foreach ( GeoSceneLayer *layer, d->m_mapTheme->map()->layers() ) {
        if ( layer->backend() != dgml::dgmlValue_geodata )
            continue;
        if ( layer->datasets().count() <= 0 )
            continue;

        // look for documents
        foreach ( GeoSceneAbstractDataset *dataset, layer->datasets() ) {
            if( dataset->fileFormat() == "KML" ) {
                QString containername = reinterpret_cast<GeoSceneXmlDataSource*>( dataset )->filename();
                if ( !loadedContainers.removeOne( containername ) ) {
                    loadList << containername;
                }
            }
        }
    }
    // unload old standard Placemarks which are not part of the new map
    foreach(const QString& container, loadedContainers) {
        loadedContainers.pop_front();
        d->m_fileManager->removeFile( container );
    }
    // load new standard Placemarks
    d->m_fileManager->addFile( loadList, MapDocument );
    loadList.clear();

    mDebug() << "THEME CHANGED: ***" << mapTheme->head()->mapThemeId();
    emit themeChanged( mapTheme->head()->mapThemeId() );
}

void MarbleModel::home( qreal &lon, qreal &lat, int& zoom ) const
{
    d->m_homePoint.geoCoordinates( lon, lat, GeoDataCoordinates::Degree );
    zoom = d->m_homeZoom;
}

void MarbleModel::setHome( qreal lon, qreal lat, int zoom )
{
    d->m_homePoint = GeoDataCoordinates( lon, lat, 0, GeoDataCoordinates::Degree );
    d->m_homeZoom = zoom;
}

void MarbleModel::setHome( const GeoDataCoordinates& homePoint, int zoom )
{
    d->m_homePoint = homePoint;
    d->m_homeZoom = zoom;
}

MapThemeManager *MarbleModel::mapThemeManager()
{
    return &d->m_mapThemeManager;
}

HttpDownloadManager *MarbleModel::downloadManager()
{
    return &d->m_downloadManager;
}

const HttpDownloadManager *MarbleModel::downloadManager() const
{
    return &d->m_downloadManager;
}


GeoDataTreeModel *MarbleModel::treeModel()
{
    return &d->m_treemodel;
}

QAbstractItemModel *MarbleModel::placemarkModel()
{
    return &d->m_sortproxy;
}

QItemSelectionModel *MarbleModel::placemarkSelectionModel()
{
    return &d->m_placemarkselectionmodel;
}

PositionTracking *MarbleModel::positionTracking() const
{
    return &d->m_positionTracking;
}

FileViewModel *MarbleModel::fileViewModel()
{
    return &d->m_fileviewmodel;
}

void MarbleModel::openGpxFile( const QString& filename )
{
    addGeoDataFile( filename );
}

void MarbleModel::addPlacemarkFile( const QString& filename )
{
    addGeoDataFile( filename );
}

void MarbleModel::addPlacemarkData( const QString& data, const QString& key )
{
    addGeoDataString( data, key );
}

void MarbleModel::removePlacemarkKey( const QString& key )
{
    removeGeoData( key );
}

FileManager *MarbleModel::fileManager()
{
    return d->m_fileManager;
}

qreal MarbleModel::planetRadius()   const
{
    return d->m_planet->radius();
}

QString MarbleModel::planetName()   const
{
    return d->m_planet->name();
}

QString MarbleModel::planetId() const
{
    return d->m_planet->id();
}

MarbleClock *MarbleModel::clock()
{
    return &d->m_clock;
}

const MarbleClock *MarbleModel::clock() const
{
    return &d->m_clock;
}

SunLocator *MarbleModel::sunLocator()
{
    return &d->m_sunLocator;
}

const SunLocator *MarbleModel::sunLocator() const
{
    return &d->m_sunLocator;
}

quint64 MarbleModel::persistentTileCacheLimit() const
{
    return d->m_storageWatcher.cacheLimit() / 1024;
}

void MarbleModel::clearPersistentTileCache()
{
    d->m_storagePolicy.clearCache();

    // Now create base tiles again if needed
    if ( d->m_mapTheme->map()->hasTextureLayers() ) {
        // If the tiles aren't already there, put up a progress dialog
        // while creating them.

        // As long as we don't have an Layer Management Class we just lookup
        // the name of the layer that has the same name as the theme ID
        QString themeID = d->m_mapTheme->head()->theme();

        GeoSceneLayer *layer =
            static_cast<GeoSceneLayer*>( d->m_mapTheme->map()->layer( themeID ) );
        GeoSceneTiled *texture =
            static_cast<GeoSceneTiled*>( layer->groundDataset() );

        QString sourceDir = texture->sourceDir();
        QString installMap = texture->installMap();
        QString role = d->m_mapTheme->map()->layer( themeID )->role();

        if ( !TileLoader::baseTilesAvailable( *texture )
            && !installMap.isEmpty() )
        {
            mDebug() << "Base tiles not available. Creating Tiles ... \n"
                     << "SourceDir: " << sourceDir << "InstallMap:" << installMap;
            MarbleDirs::debug();

            TileCreator *tileCreator = new TileCreator(
                                     sourceDir,
                                     installMap,
                                     (role == "dem") ? "true" : "false" );

            QPointer<TileCreatorDialog> tileCreatorDlg = new TileCreatorDialog( tileCreator, 0 );
            tileCreatorDlg->setSummary( d->m_mapTheme->head()->name(),
                                        d->m_mapTheme->head()->description() );
            tileCreatorDlg->exec();
            qDebug("Tile creation completed");
            delete tileCreatorDlg;
        }
    }
}

void MarbleModel::setPersistentTileCacheLimit(quint64 kiloBytes)
{
    d->m_storageWatcher.setCacheLimit( kiloBytes * 1024 );

    if( kiloBytes != 0 )
    {
        if( !d->m_storageWatcher.isRunning() )
            d->m_storageWatcher.start( QThread::IdlePriority );
    }
    else
    {
        d->m_storageWatcher.quit();
    }
    // TODO: trigger update
}

void MarbleModel::setTrackedPlacemark( const GeoDataPlacemark *placemark )
{
    d->m_trackedPlacemark = placemark;
    emit trackedPlacemarkChanged( placemark );
}

const GeoDataPlacemark* MarbleModel::trackedPlacemark() const
{
    return d->m_trackedPlacemark;
}

const PluginManager* MarbleModel::pluginManager() const
{
    return &d->m_pluginManager;
}

const Planet *MarbleModel::planet() const
{
    return d->m_planet;
}

void MarbleModel::addDownloadPolicies( GeoSceneDocument *mapTheme )
{
    if ( !mapTheme )
        return;
    if ( !mapTheme->map()->hasTextureLayers() )
        return;

    // As long as we don't have an Layer Management Class we just lookup
    // the name of the layer that has the same name as the theme ID
    const QString themeId = mapTheme->head()->theme();
    GeoSceneLayer * const layer = static_cast<GeoSceneLayer*>( mapTheme->map()->layer( themeId ));
    if ( !layer )
        return;

    GeoSceneTiled * const texture = static_cast<GeoSceneTiled*>( layer->groundDataset() );
    if ( !texture )
        return;

    QList<DownloadPolicy *> policies = texture->downloadPolicies();
    QList<DownloadPolicy *>::const_iterator pos = policies.constBegin();
    QList<DownloadPolicy *>::const_iterator const end = policies.constEnd();
    for (; pos != end; ++pos ) {
        d->m_downloadManager.addDownloadPolicy( **pos );
    }
}

RoutingManager* MarbleModel::routingManager()
{
    return d->m_routingManager;
}

const RoutingManager* MarbleModel::routingManager() const
{
    return d->m_routingManager;
}

void MarbleModel::setClockDateTime( const QDateTime& datetime )
{
    d->m_clock.setDateTime( datetime );
}

QDateTime MarbleModel::clockDateTime() const
{
    return d->m_clock.dateTime();
}

int MarbleModel::clockSpeed() const
{
    return d->m_clock.speed();
}

void MarbleModel::setClockSpeed( int speed )
{
    d->m_clock.setSpeed( speed );
}

void MarbleModel::setClockTimezone( int timeInSec )
{
    d->m_clock.setTimezone( timeInSec );
}

int MarbleModel::clockTimezone() const
{
    return d->m_clock.timezone();
}

QTextDocument * MarbleModel::legend()
{
    return d->m_legend;
}

void MarbleModel::setLegend( QTextDocument * legend )
{
    d->m_legend = legend;
}

void MarbleModel::addGeoDataFile( const QString& filename )
{
    d->m_fileManager->addFile( filename, UserDocument, true );
}

void MarbleModel::addGeoDataString( const QString& data, const QString& key )
{
    d->m_fileManager->addData( key, data, UserDocument );
}

void MarbleModel::removeGeoData( const QString& fileName )
{
    d->m_fileManager->removeFile( fileName );
}

bool MarbleModel::workOffline() const
{
    return d->m_workOffline;
}

void MarbleModel::setWorkOffline( bool workOffline )
{
    if ( d->m_workOffline != workOffline ) {
        downloadManager()->setDownloadEnabled( !workOffline );
        d->m_workOffline = workOffline;
        emit workOfflineChanged();
    }
}

ElevationModel* MarbleModel::elevationModel()
{
    return d->m_elevationModel;
}

const ElevationModel* MarbleModel::elevationModel() const
{
    return d->m_elevationModel;
}

}

#include "MarbleModel.moc"
