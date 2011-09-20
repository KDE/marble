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

#include "kdescendantsproxymodel.h"

#include "MapThemeManager.h"
#include "global.h"
#include "MarbleDebug.h"

#include "GeoSceneDocument.h"
#include "GeoSceneFilter.h"
#include "GeoSceneHead.h"
#include "GeoSceneLayer.h"
#include "GeoSceneMap.h"
#include "GeoScenePalette.h"
#include "GeoSceneTexture.h"
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

namespace Marble
{

class MarbleModelPrivate
{
 public:
    MarbleModelPrivate( MarbleModel *parent )
        : m_parent( parent ),
          m_clock(),
          m_planet( new Planet( "earth" ) ),
          m_sunLocator( &m_clock, m_planet ),
          m_pluginManager( parent ),
          m_mapThemeManager( parent ),
          m_homePoint( -9.4, 54.8, 0.0, GeoDataCoordinates::Degree ),  // Some point that tackat defined. :-)
          m_homeZoom( 1050 ),
          m_mapTheme( 0 ),
          m_storagePolicy( MarbleDirs::localPath() ),
          m_downloadManager( &m_storagePolicy, &m_pluginManager ),
          m_fileManager( 0 ),
          m_fileviewmodel(),
          m_treemodel(),
          m_descendantproxy(),
          m_sortproxy(),
          m_placemarkselectionmodel( 0 ),
          m_positionTracking( 0 ),
          m_bookmarkManager( 0 ),
          m_routingManager( 0 ),
          m_legend( 0 ),
          m_workOffline( false )
    {
        m_sortproxy.setFilterFixedString( GeoDataTypes::GeoDataPlacemarkType );
        m_sortproxy.setFilterKeyColumn( 1 );
        m_sortproxy.setSourceModel( &m_descendantproxy );
        m_descendantproxy.setSourceModel( &m_treemodel );
    }

    ~MarbleModelPrivate()
    {
    }

    MarbleModel             *m_parent;

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
    FileStorageWatcher      *m_storageWatcher;

    // Places on the map
    FileManager             *m_fileManager;

    FileViewModel            m_fileviewmodel;
    GeoDataTreeModel         m_treemodel;
    KDescendantsProxyModel   m_descendantproxy;
    QSortFilterProxyModel    m_sortproxy;

    // Selection handling
    QItemSelectionModel      m_placemarkselectionmodel;

    //Gps Stuff
    PositionTracking        *m_positionTracking;

    BookmarkManager         *m_bookmarkManager; 
    RoutingManager          *m_routingManager;
    QTextDocument           *m_legend;

    bool                     m_workOffline;
};

MarbleModel::MarbleModel( QObject *parent )
    : QObject( parent ),
      d( new MarbleModelPrivate( this ) )
{
    QTime t;
    t.start();

    connect(&d->m_treemodel, SIGNAL( dataChanged(QModelIndex,QModelIndex) ),
            this, SIGNAL( modelChanged() ) );
    connect(&d->m_treemodel, SIGNAL( layoutChanged() ),
            this, SIGNAL( modelChanged() ) );
    connect(&d->m_treemodel, SIGNAL( modelReset() ),
            this, SIGNAL( modelChanged() ) );
    connect(&d->m_treemodel, SIGNAL( treeChanged() ),
            this, SIGNAL( modelChanged() ) );

    // A new instance of FileStorageWatcher.
    // The thread will be started at setting persistent tile cache size.
    d->m_storageWatcher = new FileStorageWatcher( MarbleDirs::localPath(), this );
    connect( this, SIGNAL( themeChanged( QString ) ),
             d->m_storageWatcher, SLOT( updateTheme( QString ) ) );
    // Setting the theme to the current theme.
    d->m_storageWatcher->updateTheme( mapThemeId() );
    // connect the StoragePolicy used by the download manager to the FileStorageWatcher
    connect( &d->m_storagePolicy, SIGNAL( cleared() ),
             d->m_storageWatcher, SLOT( resetCurrentSize() ) );
    connect( &d->m_storagePolicy, SIGNAL( sizeChanged( qint64 ) ),
             d->m_storageWatcher, SLOT( addToCurrentSize( qint64 ) ) );

    d->m_fileManager = new FileManager( this );
    d->m_fileviewmodel.setFileManager( d->m_fileManager );
    connect( d->m_fileManager,    SIGNAL( fileAdded(int)),
             &d->m_fileviewmodel, SLOT( append(int)) );
    connect( d->m_fileManager,    SIGNAL( fileRemoved(int)),
             &d->m_fileviewmodel, SLOT(remove(int)) );

    d->m_positionTracking = new PositionTracking( this );

    d->m_routingManager = new RoutingManager( d->m_parent, this );

    connect(&d->m_clock,   SIGNAL( timeChanged() ),
            &d->m_sunLocator, SLOT( update() ) );
     //Initializing Bookmark manager
    d->m_bookmarkManager = new BookmarkManager();
}

MarbleModel::~MarbleModel()
{
//    mDebug() << "MarbleModel::~MarbleModel";

    delete d->m_positionTracking;
    delete d->m_bookmarkManager;

    delete d->m_fileManager;
    delete d->m_mapTheme;
    delete d->m_planet;
    delete d;

    mDebug() << "Model deleted:" << this;
}

BookmarkManager * MarbleModel::bookmarkManager() const
{
    return d->m_bookmarkManager;
}

QString MarbleModel::mapThemeId() const
{
    QString mapThemeId = "";

    if (d->m_mapTheme)
        mapThemeId = d->m_mapTheme->head()->mapThemeId();

    return mapThemeId;
}

GeoSceneDocument* MarbleModel::mapTheme() const
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

void MarbleModel::setMapTheme( GeoSceneDocument* mapTheme )
{
    GeoSceneDocument *oldTheme = d->m_mapTheme;
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

    // find the list of previous theme's geodata
    QStringList loadedContainers;
    QVector<GeoSceneLayer*>::const_iterator it;
    QVector<GeoSceneLayer*>::const_iterator end;
    if (oldTheme) {
        it = oldTheme->map()->layers().constBegin();
        end = oldTheme->map()->layers().constEnd();
        for (; it != end; ++it) {
            GeoSceneLayer* layer = *it;
            if ( layer->backend() == dgml::dgmlValue_geodata && layer->datasets().count() > 0 ) {
                // look for documents
                const QVector<GeoSceneAbstractDataset*> & datasets = layer->datasets();
                QVector<GeoSceneAbstractDataset*>::const_iterator itds = datasets.constBegin();
                QVector<GeoSceneAbstractDataset*>::const_iterator endds = datasets.constEnd();
                for (; itds != endds; ++itds) {
                    GeoSceneAbstractDataset* dataset = *itds;
                    if( dataset->fileFormat() == "KML" ) {
                        QString containername = reinterpret_cast<GeoSceneXmlDataSource*>(dataset)->filename();
                        loadedContainers <<  containername;
                    }
                }
            }
        }
    }
    QStringList loadList;
    const QVector<GeoSceneLayer*> & layers = d->m_mapTheme->map()->layers();
    it = layers.constBegin();
    end = layers.constEnd();
    for (; it != end; ++it) {
        GeoSceneLayer* layer = *it;
        if ( layer->backend() == dgml::dgmlValue_geodata && layer->datasets().count() > 0 ) {
            // look for documents
            const QVector<GeoSceneAbstractDataset*> & datasets = layer->datasets();
            QVector<GeoSceneAbstractDataset*>::const_iterator itds = datasets.constBegin();
            QVector<GeoSceneAbstractDataset*>::const_iterator endds = datasets.constEnd();
            for (; itds != endds; ++itds) {
                GeoSceneAbstractDataset* dataset = *itds;
                if( dataset->fileFormat() == "KML" ) {
                    QString containername = reinterpret_cast<GeoSceneXmlDataSource*>(dataset)->filename();
                    if ( !loadedContainers.removeOne( containername ) ) {
                        loadList << containername;
                    }
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

void MarbleModel::home( qreal &lon, qreal &lat, int& zoom )
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

MapThemeManager* MarbleModel::mapThemeManager() const
{
    return &d->m_mapThemeManager;
}

HttpDownloadManager* MarbleModel::downloadManager() const
{
    return &d->m_downloadManager;
}


GeoDataTreeModel *MarbleModel::treeModel() const
{
    return &d->m_treemodel;
}

QAbstractItemModel *MarbleModel::placemarkModel() const
{
    return &d->m_sortproxy;
}

QItemSelectionModel *MarbleModel::placemarkSelectionModel() const
{
    return &d->m_placemarkselectionmodel;
}

PositionTracking *MarbleModel::positionTracking() const
{
    return d->m_positionTracking;
}

FileViewModel *MarbleModel::fileViewModel() const
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

FileManager* MarbleModel::fileManager() const
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

SunLocator* MarbleModel::sunLocator() const
{
    return &d->m_sunLocator;
}

quint64 MarbleModel::persistentTileCacheLimit() const
{
    return d->m_storageWatcher->cacheLimit() / 1024;
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
        GeoSceneTexture *texture =
            static_cast<GeoSceneTexture*>( layer->groundDataset() );

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
    d->m_storageWatcher->setCacheLimit( kiloBytes * 1024 );

    if( kiloBytes != 0 )
    {
        if( !d->m_storageWatcher->isRunning() )
            d->m_storageWatcher->start( QThread::IdlePriority );
    }
    else
    {
        d->m_storageWatcher->quit();
    }
    // TODO: trigger update
}

PluginManager* MarbleModel::pluginManager() const
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

    GeoSceneTexture * const texture = static_cast<GeoSceneTexture*>( layer->groundDataset() );
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

}

#include "MarbleModel.moc"
