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

#include "MapThemeManager.h"
#include "global.h"
#include "MarbleDebug.h"

#include "GeoSceneDocument.h"
#include "GeoSceneFilter.h"
#include "GeoSceneHead.h"
#include "GeoSceneLayer.h"
#include "GeoSceneMap.h"
#include "GeoScenePalette.h"
#include "GeoSceneSettings.h"
#include "GeoSceneTexture.h"
#include "GeoSceneVector.h"
#include "GeoSceneXmlDataSource.h"

#include "GeoDataDocument.h"
#include "GeoDataStyle.h"

#include "DgmlAuxillaryDictionary.h"
#include "MarbleClock.h"
#include "FileStoragePolicy.h"
#include "FileStorageWatcher.h"
#include "GeoPainter.h"
#include "FileViewModel.h"
#include "PositionTracking.h"
#include "HttpDownloadManager.h"
#include "LayerManager.h"
#include "MarbleDataFacade.h"
#include "MarbleDirs.h"
#include "MarblePlacemarkModel.h"
#include "MeasureTool.h"
#include "FileManager.h"
#include "GeoDataTreeModel.h"
#include "AtmosphereLayer.h"
#include "FogLayer.h"
#include "TextureLayer.h"
#include "GeometryLayer.h"
#include "PlacemarkManager.h"
#include "PlacemarkLayout.h"
#include "PlacemarkPainter.h"
#include "Planet.h"
#include "PluginManager.h"
#include "StoragePolicy.h"
#include "SunLocator.h"
#include "StackedTile.h"
#include "TileCoordsPyramid.h"
#include "TileCreator.h"
#include "TileCreatorDialog.h"
#include "StackedTileLoader.h"
#include "VectorComposer.h"
#include "ViewParams.h"
#include "ViewportParams.h"
#include "routing/RoutingManager.h"
#include "BookmarkManager.h"

namespace Marble
{

class MarbleModelPrivate
{
 public:
    MarbleModelPrivate( MarbleModel *parent )
        : m_parent( parent ),
          m_dataFacade( 0 ),
          m_clock( new MarbleClock() ),
          m_planet( new Planet( "earth" ) ),
          m_sunLocator( new SunLocator( m_clock, m_planet ) ),
          m_pluginManager( new PluginManager( parent ) ),
          m_mapThemeManager( new MapThemeManager( parent )),
          m_homePoint( -9.4, 54.8, 0.0, GeoDataCoordinates::Degree ),  // Some point that tackat defined. :-)
          m_homeZoom( 1050 ),
          m_mapTheme( 0 ),
          m_layerManager( 0 ),
          m_downloadManager( new HttpDownloadManager( new FileStoragePolicy(
                                                                   MarbleDirs::localPath() ),
                                                      m_pluginManager ) ),
          m_fileManager( 0 ),
          m_placemarkmanager( 0 ),
          m_placemarkLayout( 0 ),
          m_popSortModel( parent ),
          m_textureLayer( new TextureLayer( m_mapThemeManager, m_downloadManager, m_sunLocator ) ),
          m_placemarkselectionmodel( &m_popSortModel ),
          m_positionTracking( 0 ),
          m_bookmarkManager( 0 ),
          m_routingManager( 0 ),
          m_legend( 0 ),
          m_backgroundVisible( true )
    {
    }

    ~MarbleModelPrivate()
    {
        delete m_downloadManager;
    }

    void notifyModelChanged();

    MarbleModel             *m_parent;
    MarbleDataFacade        *m_dataFacade;

    // Misc stuff.
    MarbleClock       *const m_clock;
    Planet                  *m_planet;
    SunLocator        *const m_sunLocator;

    PluginManager           *m_pluginManager;
    MapThemeManager         *m_mapThemeManager;

    // The home position
    GeoDataCoordinates       m_homePoint;
    int                      m_homeZoom;

    // View and paint stuff
    GeoSceneDocument        *m_mapTheme;
    LayerManager            *m_layerManager;

    HttpDownloadManager     *m_downloadManager;

    // Cache related
    FileStorageWatcher      *m_storageWatcher;

    VectorComposer           m_veccomposer;

    // Tools
    MeasureTool             *m_measureTool;

    // Places on the map
    FileManager             *m_fileManager;
    PlacemarkManager        *m_placemarkmanager;
    PlacemarkLayout         *m_placemarkLayout;
    QSortFilterProxyModel    m_popSortModel;
    GeometryLayer           *m_geometryLayer;
    AtmosphereLayer          m_atmosphereLayer;
    FogLayer                 m_fogLayer;
    TextureLayer            *m_textureLayer;

    // Selection handling
    QItemSelectionModel      m_placemarkselectionmodel;

    //Gps Stuff
    PositionTracking        *m_positionTracking;

    BookmarkManager         *m_bookmarkManager; 
    RoutingManager          *m_routingManager;
    QTextDocument           *m_legend;
    bool                     m_backgroundVisible;

};

MarbleModel::MarbleModel( QObject *parent )
    : QObject( parent ),
      d( new MarbleModelPrivate( this ) )
{
    QTime t;
    t.start();

    connect( &d->m_veccomposer, SIGNAL( datasetLoaded() ), SIGNAL( modelChanged() ) );
    connect( d->m_textureLayer, SIGNAL( modelChanged() ), SIGNAL( modelChanged() ) );

    d->m_dataFacade = new MarbleDataFacade( this );
    connect(d->m_dataFacade->treeModel(), SIGNAL( dataChanged(QModelIndex,QModelIndex) ),
            this, SIGNAL( modelChanged() ) );

    // A new instance of FileStorageWatcher.
    // The thread will be started at setting persistent tile cache size.
    d->m_storageWatcher = new FileStorageWatcher( MarbleDirs::localPath(), this );
    connect( this, SIGNAL( themeChanged( QString ) ),
             d->m_storageWatcher, SLOT( updateTheme( QString ) ) );
    // Setting the theme to the current theme.
    d->m_storageWatcher->updateTheme( mapThemeId() );
    // connect the StoragePolicy used by the download manager to the FileStorageWatcher
    StoragePolicy * const storagePolicy = d->m_downloadManager->storagePolicy();
    connect( storagePolicy, SIGNAL( cleared() ),
             d->m_storageWatcher, SLOT( resetCurrentSize() ) );
    connect( storagePolicy, SIGNAL( sizeChanged( qint64 ) ),
             d->m_storageWatcher, SLOT( addToCurrentSize( qint64 ) ) );

    d->m_fileManager = new FileManager();
    d->m_fileManager->setDataFacade(d->m_dataFacade);

    d->m_placemarkmanager = new PlacemarkManager();
    d->m_placemarkmanager->setDataFacade(d->m_dataFacade);
    d->m_placemarkmanager->setFileManager(d->m_fileManager);

    d->m_measureTool = new MeasureTool( this );

    d->m_popSortModel.setSourceModel( d->m_dataFacade->placemarkModel() );
//    d->m_popSortModel->setSortLocaleAware( true );
    d->m_popSortModel.setDynamicSortFilter( true );
    d->m_popSortModel.setSortRole( MarblePlacemarkModel::PopularityIndexRole );
    d->m_popSortModel.sort( 0, Qt::DescendingOrder );

    d->m_placemarkLayout = new PlacemarkLayout( &d->m_popSortModel, &d->m_placemarkselectionmodel, this );
    connect( &d->m_placemarkselectionmodel,  SIGNAL( selectionChanged( QItemSelection,
                                                                      QItemSelection) ),
             d->m_placemarkLayout,          SLOT( requestStyleReset() ) );
    connect( &d->m_popSortModel,           SIGNAL( layoutChanged() ),
             d->m_placemarkLayout,          SLOT( requestStyleReset() ) );

    /*
     * Create FileViewModel
     */
    connect( d->m_dataFacade->fileViewModel(), SIGNAL( modelChanged() ),
             this,            SIGNAL( modelChanged() ) );

    d->m_positionTracking = new PositionTracking( d->m_fileManager, this );

    d->m_layerManager = new LayerManager( d->m_dataFacade, d->m_pluginManager, this );

    // FIXME: more on the spot update names and API
    connect ( d->m_layerManager,      SIGNAL( floatItemsChanged() ),
              this,                   SIGNAL( modelChanged() ) );

    connect ( d->m_layerManager, SIGNAL( pluginSettingsChanged() ),
              this,              SIGNAL( pluginSettingsChanged() ) );
    connect ( d->m_layerManager, SIGNAL( repaintNeeded( QRegion ) ),
              this,              SIGNAL( repaintNeeded( QRegion ) ) );
    connect ( d->m_layerManager, SIGNAL( renderPluginInitialized( RenderPlugin * ) ),
              this,              SIGNAL( renderPluginInitialized( RenderPlugin * ) ) );

    GeoDataObject *object = static_cast<GeoDataObject*>(d->m_dataFacade->treeModel()->index(0, 0, QModelIndex()).internalPointer());
    GeoDataDocument *document = dynamic_cast<GeoDataDocument*>( object->parent() );
    d->m_geometryLayer = new GeometryLayer( document );
    d->m_layerManager->addLayer( d->m_geometryLayer );

    d->m_routingManager = new RoutingManager( d->m_parent, this );

    connect(d->m_clock,   SIGNAL( timeChanged() ),
            d->m_sunLocator, SLOT( update() ) );
     //Initializing Bookmark manager
    d->m_bookmarkManager = new BookmarkManager();
}

MarbleModel::~MarbleModel()
{
//    mDebug() << "MarbleModel::~MarbleModel";

    delete d->m_positionTracking;
    delete d->m_bookmarkManager;

    delete d->m_placemarkmanager;
    delete d->m_fileManager;
    delete d->m_mapTheme;
    delete d->m_layerManager;
    delete d->m_dataFacade;
    delete d->m_sunLocator;
    delete d->m_planet;
    delete d->m_clock;
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

void MarbleModel::setMapTheme( GeoSceneDocument* mapTheme,
                               Projection currentProjection )
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

    d->m_textureLayer->setMapTheme( d->m_mapTheme );
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

        if ( !StackedTileLoader::baseTilesAvailable( layer )
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

        d->m_textureLayer->setupTextureMapper( currentProjection );
    }

    // Set all the colors for the vector layers
    if ( d->m_mapTheme->map()->hasVectorLayers() ) {
        d->m_veccomposer.setOceanColor( d->m_mapTheme->map()->backgroundColor() );

        // Just as with textures, this is a workaround for DGML2 to
        // emulate the old behaviour.

        GeoSceneLayer *layer = d->m_mapTheme->map()->layer( "mwdbii" );
        if ( layer ) {
            GeoSceneVector *vector = 0;

            vector = static_cast<GeoSceneVector*>( layer->dataset("pdiffborder") );
            if ( vector )
                d->m_veccomposer.setCountryBorderColor( vector->pen().color() );

            vector = static_cast<GeoSceneVector*>( layer->dataset("rivers") );
            if ( vector )
                d->m_veccomposer.setRiverColor( vector->pen().color() );

            vector = static_cast<GeoSceneVector*>( layer->dataset("pusa48") );
            if ( vector )
                d->m_veccomposer.setStateBorderColor( vector->pen().color() );

            vector = static_cast<GeoSceneVector*>( layer->dataset("plake") );
            if ( vector )
                d->m_veccomposer.setLakeColor( vector->pen().color() );

            vector = static_cast<GeoSceneVector*>( layer->dataset("pcoast") );
            if ( vector )
            {
                d->m_veccomposer.setLandColor( vector->brush().color() );
                d->m_veccomposer.setCoastColor( vector->pen().color() );
            }
        }
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
                        if( containername.endsWith(".kml") ) containername.remove(".kml");
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
                    if( containername.endsWith(".kml") ) containername.remove(".kml");
                    loadedContainers.removeOne( containername );
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
    foreach(const QString& container, loadList) {
        loadList.pop_front();
        d->m_fileManager->addFile( container );
    }
    d->notifyModelChanged();
    d->m_placemarkLayout->requestStyleReset();


    // FIXME: Still needs to get fixed for the DGML2 refactoring
//    d->m_placemarkLayout->placemarkPainter()->setDefaultLabelColor( d->m_maptheme->labelColor() );

    mDebug() << "THEME CHANGED: ***" << mapTheme->head()->mapThemeId();
    emit themeChanged( mapTheme->head()->mapThemeId() );

    d->m_layerManager->syncViewParamsAndPlugins( mapTheme );

    d->notifyModelChanged();
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

HttpDownloadManager* MarbleModel::downloadManager() const
{
    return d->m_downloadManager;
}


void MarbleModel::paintGlobe( GeoPainter *painter,
                              ViewParams *viewParams,
                              bool redrawBackground,
                              const QRect& dirtyRect )
{
    QStringList renderPositions;

    if ( d->m_backgroundVisible ) {
        renderPositions << "STARS" << "BEHIND_TARGET";
        d->m_layerManager->renderLayers( painter, viewParams, renderPositions );
    }

    if ( viewParams->showAtmosphere() ) {
        d->m_atmosphereLayer.render( painter, viewParams->viewport() );
    }

    d->m_textureLayer->paintGlobe( painter, viewParams, redrawBackground, dirtyRect );

    renderPositions.clear();
    renderPositions << "SURFACE";

    // Paint the vector layer.
    if ( d->m_mapTheme->map()->hasVectorLayers() ) {

        if ( !d->m_mapTheme->map()->hasTextureLayers() ) {
            d->m_veccomposer.paintBaseVectorMap( painter, viewParams );
        }

        d->m_layerManager->renderLayers( painter, viewParams, renderPositions );

        // Add further Vectors
        d->m_veccomposer.paintVectorMap( painter, viewParams );
    }
    else {
        d->m_layerManager->renderLayers( painter, viewParams, renderPositions );
    }

    // Paint the GeoDataPlacemark layer
    bool showPlaces, showCities, showTerrain, showOtherPlaces;

    viewParams->propertyValue( "places", showPlaces );
    viewParams->propertyValue( "cities", showCities );
    viewParams->propertyValue( "terrain", showTerrain );
    viewParams->propertyValue( "otherplaces", showOtherPlaces );

    if ( ( showPlaces || showCities || showTerrain || showOtherPlaces )
         && d->m_popSortModel.rowCount() > 0 )
    {
        d->m_placemarkLayout->paintPlaceFolder( painter, viewParams );
    }

    renderPositions.clear();
    renderPositions << "HOVERS_ABOVE_SURFACE";
    d->m_layerManager->renderLayers( painter, viewParams, renderPositions );

    // FIXME: This is really slow. That's why we defer this to
    //        PrintQuality. Either cache on a pixmap - or maybe
    //        better: Add to GlobeScanlineTextureMapper.

    if ( viewParams->mapQuality() == PrintQuality )
        d->m_fogLayer.render( painter, viewParams->viewport() );

    renderPositions.clear();
    renderPositions << "ATMOSPHERE"
                    << "ORBIT" << "ALWAYS_ON_TOP" << "FLOAT_ITEM" << "USER_TOOLS";

    d->m_layerManager->renderLayers( painter, viewParams, renderPositions );
}


QAbstractItemModel *MarbleModel::treeModel() const
{
    return d->m_dataFacade->treeModel();
}

QAbstractItemModel *MarbleModel::placemarkModel() const
{
    return d->m_dataFacade->placemarkModel();
}

QItemSelectionModel *MarbleModel::placemarkSelectionModel() const
{
    return &d->m_placemarkselectionmodel;
}

PlacemarkLayout *MarbleModel::placemarkLayout() const
{
    return d->m_placemarkLayout;
}

PositionTracking *MarbleModel::positionTracking() const
{
    return d->m_positionTracking;
}

FileViewModel *MarbleModel::fileViewModel() const
{
    return d->m_dataFacade->fileViewModel();
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

QVector<QModelIndex> MarbleModel::whichFeatureAt( const QPoint& curpos ) const
{
    return d->m_placemarkLayout->whichPlacemarkAt( curpos );
}

void MarbleModelPrivate::notifyModelChanged()
{
    emit m_parent->modelChanged();
}

void MarbleModel::update()
{
    mDebug() << "MarbleModel::update()";
    QTimer::singleShot( 0, d->m_textureLayer->tileLoader(), SLOT( update() ) );
}

qreal MarbleModel::planetRadius()   const
{
    return d->m_planet->radius();
}

QString MarbleModel::planetName()   const
{
    return d->m_planet->name();
}

MarbleClock* MarbleModel::clock() const
{
//    mDebug() << "In dateTime, model:" << this;
//    mDebug() << d << ":" << d->m_clock;
    return d->m_clock;
}

SunLocator* MarbleModel::sunLocator() const
{
    return d->m_sunLocator;
}

void MarbleModel::setShowTileId( bool show )
{
    d->m_textureLayer->setShowTileId( show );
}

quint64 MarbleModel::persistentTileCacheLimit() const
{
    return d->m_storageWatcher->cacheLimit() / 1024;
}

void MarbleModel::clearVolatileTileCache()
{
    d->m_textureLayer->tileLoader()->update();
    mDebug() << "Cleared Volatile Cache!";
}

quint64 MarbleModel::volatileTileCacheLimit() const
{
    return d->m_textureLayer->tileLoader()->volatileCacheLimit();
}

void MarbleModel::setVolatileTileCacheLimit( quint64 kiloBytes )
{
    d->m_textureLayer->tileLoader()->setVolatileCacheLimit( kiloBytes );
}

void MarbleModel::clearPersistentTileCache()
{
    downloadManager()->storagePolicy()->clearCache();

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

        if ( !StackedTileLoader::baseTilesAvailable( layer )
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
    return d->m_pluginManager;
}

QList<RenderPlugin *> MarbleModel::renderPlugins() const
{
    return d->m_layerManager->renderPlugins();
}

QList<AbstractFloatItem *> MarbleModel::floatItems() const
{
    return d->m_layerManager->floatItems();
}

QList<AbstractDataPlugin *> MarbleModel::dataPlugins()  const
{
    return d->m_layerManager->dataPlugins();
}

QList<AbstractDataPluginItem *> MarbleModel::whichItemAt( const QPoint& curpos ) const
{
    return d->m_layerManager->whichItemAt( curpos );
}

void MarbleModel::addLayer( LayerInterface *layer )
{
    d->m_layerManager->addLayer(layer);
}

void MarbleModel::removeLayer( LayerInterface *layer )
{
    d->m_layerManager->removeLayer(layer);
}

MeasureTool *MarbleModel::measureTool()
{
    return d->m_measureTool;
}

Planet* MarbleModel::planet() const
{
    return d->m_planet;
}

int MarbleModel::tileZoomLevel() const
{
    return d->m_textureLayer->tileZoomLevel();
}

void MarbleModel::reloadMap() const
{
    Q_ASSERT( d->m_textureLayer->tileLoader() );
    QList<TileId> displayed = d->m_textureLayer->tileLoader()->tilesOnDisplay();
    QList<TileId>::const_iterator pos = displayed.constBegin();
    QList<TileId>::const_iterator const end = displayed.constEnd();
    for (; pos != end; ++pos ) {
        // it's debatable here, whether DownloadBulk or DownloadBrowse should be used
        // but since "reload" or "refresh" seems to be a common action of a browser and it
        // allows for more connections (in our model), use "DownloadBrowse"
        d->m_textureLayer->tileLoader()->reloadTile( *pos, DownloadBrowse );
    }
}

void MarbleModel::downloadRegion( QString const & mapThemeId,
                                  QVector<TileCoordsPyramid> const & pyramid ) const
{
    Q_ASSERT( d->m_textureLayer->tileLoader() );
    Q_ASSERT( !pyramid.isEmpty() );
    QTime t;
    t.start();

    // When downloading a region (the author of these lines thinks) most users probably expect
    // the download to begin with the low resolution tiles and then procede level-wise to
    // higher resolution tiles. In order to achieve this, we start requesting downloads of
    // high resolution tiles and request the low resolution tiles at the end because
    // DownloadQueueSet (silly name) is implemented as stack.


    int const first = 0;
    int tilesCount = 0;

     for ( int level = pyramid[first].bottomLevel(); level >= pyramid[first].topLevel(); --level ) {
         QSet<TileId> tileIdSet;
          for( int i = 0; i < pyramid.size(); ++i ) {
            QRect const coords = pyramid[i].coords( level );
            mDebug() << "MarbleModel::downloadRegion level:" << level << "tile coords:" << coords;
            int x1, y1, x2, y2;
            coords.getCoords( &x1, &y1, &x2, &y2 );
            for ( int x = x1; x <= x2; ++x ) {
                for ( int y = y1; y <= y2; ++y ) {
                    TileId const tileId( mapThemeId, level, x, y );
                    tileIdSet.insert( tileId );
                    // FIXME: use lazy evaluation to not generate up to 100k tiles in one go
                    // this can take considerable time even on very fast systems
                    // in contrast generating the TileIds on the fly when they are needed
                    // does not seem to affect download speed.
                }
            }
         }
         QSetIterator<TileId> i( tileIdSet );
         while( i.hasNext() ) {
              d->m_textureLayer->tileLoader()->downloadTile( i.next() );
         }
         tilesCount += tileIdSet.count();
     }
    // Needed for downloading unique tiles only. Much faster than if tiles for each level is downloaded separately

    int const elapsedMs = t.elapsed();
    mDebug() << "MarbleModel::downloadRegion:" << tilesCount << "tiles, " << elapsedMs << "ms";
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
        d->m_downloadManager->addDownloadPolicy( **pos );
    }
}

// this method will only temporarily "pollute" the MarbleModel class
TextureLayer* MarbleModel::textureLayer()
{
    return d->m_textureLayer;
}

RoutingManager* MarbleModel::routingManager()
{
    return d->m_routingManager;
}

void MarbleModel::setClockDateTime( const QDateTime& datetime )
{
    d->m_clock->setDateTime( datetime );
}

QDateTime MarbleModel::clockDateTime() const
{
    return d->m_clock->dateTime();
}

int MarbleModel::clockSpeed() const
{
    return d->m_clock->speed();
}

void MarbleModel::setClockSpeed( int speed )
{
    d->m_clock->setSpeed( speed );
}

void MarbleModel::setClockTimezone( int timeInSec )
{
    d->m_clock->setTimezone( timeInSec );
}

int MarbleModel::clockTimezone() const
{
    return d->m_clock->timezone();
}

QTextDocument * MarbleModel::legend()
{
    return d->m_legend;
}

void MarbleModel::setLegend( QTextDocument * legend )
{
    d->m_legend = legend;
}

bool MarbleModel::backgroundVisible() const
{
    return d->m_backgroundVisible;
}

void MarbleModel::setBackgroundVisible( bool visible )
{
    d->m_backgroundVisible = visible;
}

void MarbleModel::addGeoDataFile( const QString& filename )
{
    d->m_fileManager->addFile( filename );

    d->notifyModelChanged();
}

void MarbleModel::addGeoDataString( const QString& data, const QString& key )
{
    d->m_fileManager->addData( key, data );

    d->notifyModelChanged();
}

void MarbleModel::removeGeoData( const QString& fileName )
{
    d->m_fileManager->removeFile( fileName );

    d->notifyModelChanged();
}

}

#include "MarbleModel.moc"
