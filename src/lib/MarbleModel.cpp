//
// This file is part of the Marble Desktop Globe.
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
#include <QtGui/QItemSelectionModel>
#include <QtGui/QSortFilterProxyModel>

#include "MapThemeManager.h"
#include "global.h"
#include "MarbleDebug.h"
#include "gps/GpsLayer.h"

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
#include "ExtDateTime.h"
#include "FileStoragePolicy.h"
#include "GeoPainter.h"
#include "FileViewModel.h"
#include "SphericalScanlineTextureMapper.h"
#include "EquirectScanlineTextureMapper.h"
#include "MercatorScanlineTextureMapper.h"
#include "gps/GpxFileModel.h"
#include "gps/GpxFile.h"
#include "HttpDownloadManager.h"
#include "KmlFileViewItem.h"
#include "LayerManager.h"
#include "MarbleDataFacade.h"
#include "MarbleDirs.h"
#include "MarblePlacemarkModel.h"
#include "MarbleGeometryModel.h"
#include "MergedLayerDecorator.h"
#include "FileManager.h"
#include "PlacemarkManager.h"
#include "PlacemarkLayout.h"
#include "PlacemarkPainter.h"
#include "Planet.h"
#include "PluginManager.h"
#include "StoragePolicy.h"
#include "SunLocator.h"
#include "TextureColorizer.h"
#include "StackedTile.h"
#include "TileCoordsPyramid.h"
#include "TileCreator.h"
#include "TileCreatorDialog.h"
#include "StackedTileLoader.h"
#include "VectorComposer.h"
#include "ViewParams.h"
#include "ViewportParams.h"

namespace Marble
{

class MarbleModelPrivate
{
 public:
    MarbleModelPrivate( MarbleModel *parent )
        : m_parent( parent ),
          m_dataFacade( 0 ),
          m_pluginManager( new PluginManager( parent ) ),
          m_mapThemeManager( new MapThemeManager( parent )),
          m_mapTheme( 0 ),
          m_layerManager( 0 ),
          m_downloadManager( new HttpDownloadManager( new FileStoragePolicy(
                                                                   MarbleDirs::localPath() ),
                                                      m_pluginManager ) ),
          m_fileManager( 0 ),
          m_placemarkmanager( 0 )
    {
    }

    ~MarbleModelPrivate()
    {
        delete m_downloadManager;
    }

    void resize( int width, int height );
    void notifyModelChanged();
    GeoSceneGroup * textureLayerProperties() const;

    void drawFog( QPainter *painter, ViewParams *viewParams, int width, int height );

    static QAtomicInt       refCounter;
    MarbleModel             *m_parent;
    MarbleDataFacade        *m_dataFacade;

    PluginManager           *m_pluginManager;
    MapThemeManager         *m_mapThemeManager;

    // View and paint stuff
    GeoSceneDocument        *m_mapTheme;
    LayerManager            *m_layerManager;
    static TextureColorizer *m_texcolorizer; //left as null if unused

    HttpDownloadManager     *m_downloadManager;

    StackedTileLoader       *m_tileLoader;
    AbstractScanlineTextureMapper   *m_texmapper;

    static VectorComposer   *m_veccomposer; // FIXME: Make not a pointer.

    // Places on the map
    FileManager             *m_fileManager;
    PlacemarkManager        *m_placemarkmanager;
    PlacemarkLayout         *m_placemarkLayout;
    QSortFilterProxyModel   *m_popSortModel;

    // Misc stuff.
    ExtDateTime             *m_dateTime;
    SunLocator              *m_sunLocator;
    MergedLayerDecorator    *m_layerDecorator;

    // Selection handling
    QItemSelectionModel     *m_placemarkselectionmodel;

    //Gps Stuff
    GpsLayer                *m_gpsLayer;
    GpxFileModel            *m_gpxFileModel;

    QTimer                  *m_timer;


    Planet                  *m_planet;
};

VectorComposer      *MarbleModelPrivate::m_veccomposer = 0;
TextureColorizer    *MarbleModelPrivate::m_texcolorizer = 0;
QAtomicInt           MarbleModelPrivate::refCounter(0);

GeoSceneGroup * MarbleModelPrivate::textureLayerProperties() const
{
    if ( !m_mapTheme )
        return 0;

    GeoSceneSettings * const settings = m_mapTheme->settings();
    if ( !settings )
        return 0;

    return settings->group( "Texture Layers" );
}

MarbleModel::MarbleModel( QObject *parent )
    : QObject( parent ),
      d( new MarbleModelPrivate( this ) )
{
    QTime t;
    t.start();
    MarbleModelPrivate::refCounter.ref();
    d->m_dataFacade = new MarbleDataFacade( this );

    d->m_tileLoader = new StackedTileLoader( d->m_mapThemeManager, d->textureLayerProperties(),
                                             d->m_downloadManager, this );
    d->m_texmapper = 0;
    
    d->m_fileManager = new FileManager();
    d->m_fileManager->setDataFacade(d->m_dataFacade);

    d->m_placemarkmanager = new PlacemarkManager();
    d->m_placemarkmanager->setDataFacade(d->m_dataFacade);
    d->m_placemarkmanager->setFileManager(d->m_fileManager);

    d->m_popSortModel = new QSortFilterProxyModel( this );

    d->m_popSortModel->setSourceModel( d->m_dataFacade->placemarkModel() );
//    d->m_popSortModel->setSortLocaleAware( true );
//    d->m_popSortModel->setDynamicSortFilter( true );
    d->m_popSortModel->setSortRole( MarblePlacemarkModel::PopularityIndexRole );
    d->m_popSortModel->sort( 0, Qt::DescendingOrder );
    
    d->m_placemarkselectionmodel = new QItemSelectionModel( d->m_popSortModel );

    d->m_placemarkLayout = new PlacemarkLayout( this );
    connect( d->m_placemarkselectionmodel,  SIGNAL( selectionChanged( QItemSelection,
                                                                      QItemSelection) ),
             d->m_placemarkLayout,          SLOT( requestStyleReset() ) );
    connect( d->m_dataFacade->placemarkModel(),           SIGNAL( layoutChanged() ),
             d->m_placemarkLayout,          SLOT( requestStyleReset() ) );

    /*
     * Create FileViewModel
     */
    connect( d->m_dataFacade->fileViewModel(), SIGNAL( modelChanged() ),
             this,            SIGNAL( modelChanged() ) );

    d->m_gpxFileModel = new GpxFileModel( this );
    d->m_gpsLayer = new GpsLayer( d->m_gpxFileModel );

    connect( d->m_dataFacade->fileViewModel(), SIGNAL(layoutChanged()),
             d->m_gpsLayer, SLOT(clearModel() ) );

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

    d->m_timer = new QTimer( this );
    d->m_timer->start( 200 );

    connect( d->m_timer, SIGNAL( timeout() ),
             this,       SIGNAL( timeout() ) );

    d->m_dateTime       = new ExtDateTime();
    /* Assume we are dealing with the earth */
    d->m_planet = new Planet( "earth" );
    d->m_sunLocator     = new SunLocator( d->m_dateTime, d->m_planet );
    d->m_layerDecorator = new MergedLayerDecorator( d->m_tileLoader, d->m_sunLocator );

    connect(d->m_dateTime,   SIGNAL( timeChanged() ),
            d->m_sunLocator, SLOT( update() ) );
    connect( d->m_layerDecorator, SIGNAL( repaintMap() ),
                                  SIGNAL( modelChanged() ) );
}

MarbleModel::~MarbleModel()
{
//    mDebug() << "MarbleModel::~MarbleModel";
    
    delete d->m_texmapper;

    delete d->m_tileLoader; // disconnects from downloadManager in dtor

    if( MarbleModelPrivate::refCounter == 1 ) {
        delete d->m_veccomposer;
        delete d->m_texcolorizer;
    }
    delete d->m_popSortModel;
    delete d->m_placemarkmanager;
    delete d->m_fileManager;
    delete d->m_gpsLayer;
    delete d->m_mapTheme;
    delete d->m_timer;
    delete d->m_layerManager;
    delete d->m_dataFacade;
    delete d->m_layerDecorator;
    delete d->m_sunLocator;
    delete d->m_dateTime;
    delete d->m_planet;
    delete d;
    MarbleModelPrivate::refCounter.deref();
    mDebug() << "Model deleted:" << this;
}

bool MarbleModel::showGps() const
{
    return d->m_gpsLayer->visible();
}

void MarbleModel::setShowGps( bool visible )
{
    d->m_gpsLayer->setVisible( visible );
}

void MarbleModel::startPolling( int time )
{
    d->m_timer->start( time );
}

void MarbleModel::stopPolling()
{
    d->m_timer->stop();
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
    d->m_mapTheme = mapTheme;
    addDownloadPolicies( d->m_mapTheme );
    d->m_tileLoader->setTextureLayerSettings( d->textureLayerProperties() );

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
    else {
        d->m_tileLoader->flush();
    }

    setupTextureMapper( currentProjection );

    // Set all the colors for the vector layers
    if ( d->m_mapTheme->map()->hasVectorLayers() && d->m_veccomposer ) {
        d->m_veccomposer->setOceanColor( d->m_mapTheme->map()->backgroundColor() );
        
        // Just as with textures, this is a workaround for DGML2 to
        // emulate the old behaviour.

        GeoSceneLayer *layer = d->m_mapTheme->map()->layer( "mwdbii" );
        if ( layer ) {
            GeoSceneVector *vector = 0;

            vector = static_cast<GeoSceneVector*>( layer->dataset("pdiffborder") );
            if ( vector )
                d->m_veccomposer->setCountryBorderColor( vector->pen().color() );

            vector = static_cast<GeoSceneVector*>( layer->dataset("rivers") );
            if ( vector )
                d->m_veccomposer->setRiverColor( vector->pen().color() );

            vector = static_cast<GeoSceneVector*>( layer->dataset("pusa48") );
            if ( vector )
                d->m_veccomposer->setStateBorderColor( vector->pen().color() );

            vector = static_cast<GeoSceneVector*>( layer->dataset("plake") );
            if ( vector )
                d->m_veccomposer->setLakeColor( vector->pen().color() );

            vector = static_cast<GeoSceneVector*>( layer->dataset("pcoast") );
            if ( vector )
            {
                d->m_veccomposer->setLandColor( vector->brush().color() );
                d->m_veccomposer->setCoastColor( vector->pen().color() );
            }
        }
    }
    d->m_dataFacade->geometryModel()->setGeoDataRoot( 0 );
    QStringList loadedContainers = d->m_fileManager->containers();
    QStringList loadList;
    const QVector<GeoSceneLayer*> & layers = d->m_mapTheme->map()->layers();
    QVector<GeoSceneLayer*>::const_iterator it = layers.constBegin();
    QVector<GeoSceneLayer*>::const_iterator end = layers.constEnd();
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

    if( !d->m_mapTheme->map()->filters().isEmpty() ) {
        GeoSceneFilter *filter= d->m_mapTheme->map()->filters().first();

        if( filter->type() == "colorize" ) {
             //no need to look up with MarbleDirs twice so they are left null for now
            QString seafile, landfile; 
            QList<GeoScenePalette*> palette = filter->palette();
            foreach ( GeoScenePalette *curPalette, palette ) {
                if( curPalette->type() == "sea" ) {
                    seafile = MarbleDirs::path( curPalette->file() );
                } else if( curPalette->type() == "land" ) {
                    landfile = MarbleDirs::path( curPalette->file() );
                }
            }
            //look up locations if they are empty
            if(seafile.isEmpty())
                seafile = MarbleDirs::path( "seacolors.leg" );
            if(landfile.isEmpty())
                landfile = MarbleDirs::path( "landcolors.leg" );

            if( !d->m_texcolorizer ) {
                /* This is where the TextureColorizer is created if it's needed
                   by the new map theme. */
                d->m_texcolorizer = new TextureColorizer( seafile, landfile );
            } else if( d->m_texcolorizer->seafile() != seafile ||
                       d->m_texcolorizer->landfile() != landfile ) {
                d->m_texcolorizer->generatePalette( seafile, landfile );
            }
        }
    }
    mDebug() << "THEME CHANGED: ***" << mapTheme->head()->mapThemeId();
    emit themeChanged( mapTheme->head()->mapThemeId() );

    d->m_layerManager->syncViewParamsAndPlugins( mapTheme );

    d->notifyModelChanged();
}

void MarbleModel::setupTextureMapper( Projection projection )
{
  // FIXME: replace this with an approach based on the factory method pattern.
    delete d->m_texmapper;

    switch( projection ) {
        case Spherical:
            d->m_texmapper = new SphericalScanlineTextureMapper( textureLayer(), d->m_tileLoader,
                                                                 this );
            break;
        case Equirectangular:
            d->m_texmapper = new EquirectScanlineTextureMapper( textureLayer(), d->m_tileLoader,
                                                                this );
            break;
        case Mercator:
            d->m_texmapper = new MercatorScanlineTextureMapper( textureLayer(), d->m_tileLoader,
                                                                this );
            break;
        default:
            d->m_texmapper = 0;
    }
    Q_ASSERT( d->m_texmapper );
    connect( d->m_texmapper, SIGNAL( tileLevelChanged( int )), SIGNAL( tileLevelChanged( int )));
    connect( d->m_texmapper, SIGNAL( mapChanged() ),
             this,           SLOT( notifyModelChanged() ) );
}

void MarbleModel::setupVectorComposer()
{
    if( MarbleModelPrivate::refCounter == 1 ) {
        d->m_veccomposer = new VectorComposer();
        connect( d->m_veccomposer, SIGNAL( datasetLoaded() ), SIGNAL( modelChanged() ) );
        d->m_texcolorizer = 0;
        /* d->m_texcolorizer is not initialized here since it takes a long time
           to create the palette and it might not even be used. Instead it's created
           in setMapTheme if the theme being loaded does need it. If the theme
           doesn't need it, it's left as is. */
    }    
}

HttpDownloadManager* MarbleModel::downloadManager() const
{
    return d->m_downloadManager;
}

void MarbleModelPrivate::resize( int width, int height )
{
    if ( m_mapTheme->map()->hasTextureLayers() ) {
        m_texmapper->resizeMap( width, height );
    }
    if ( m_veccomposer ) {
        m_veccomposer->resizeMap( width, height );
    }
}


void MarbleModel::paintGlobe( GeoPainter *painter,
                              int width, int height,
                              ViewParams *viewParams,
                              bool redrawBackground,
                              const QRect& dirtyRect )
{
    if ( !d->m_texmapper ) return;

    d->resize( width, height );

    // FIXME: Remove this once the LMC is there:
    QString themeID = d->m_mapTheme->head()->theme();

    GeoSceneLayer *layer = 
        static_cast<GeoSceneLayer*>( d->m_mapTheme->map()->layer( themeID ) );

    QStringList renderPositions;
    renderPositions << "STARS" << "BEHIND_TARGET";
    d->m_layerManager->renderLayers( painter, viewParams, renderPositions );
        
    if ( redrawBackground ) {
        if ( d->m_mapTheme->map()->hasTextureLayers() ) {

            // Create the height map image a.k.a viewParams->m_canvasImage.
            d->m_texmapper->mapTexture( viewParams );

            if ( d->m_veccomposer
                && !viewParams->showElevationModel()
                && layer->role() == "dem"
                && !d->m_mapTheme->map()->filters().isEmpty() ) {

                GeoSceneFilter *filter= d->m_mapTheme->map()->filters().first();
                viewParams->coastImage()->fill( Qt::transparent );
                // Create VectorMap
                d->m_veccomposer->drawTextureMap( viewParams );

                // Colorize using settings from when the map was loaded
                // there's no need to check the palette because it's set with the map theme
                if( filter->type() == "colorize" ) {
                    d->m_texcolorizer->colorize( viewParams );
                }
            } //else { mDebug() << "No filters to act on..."; }
        }
    }

    // Paint the map on the Widget
//    QTime t;
//    t.start();
    int radius = (int)(1.05 * (qreal)(viewParams->radius()));

    if ( d->m_mapTheme->map()->hasTextureLayers() ) {
        if ( viewParams->projection() == Spherical ) {
            QRect rect( width / 2 - radius, height / 2 - radius,
                        2 * radius, 2 * radius);
            rect = rect.intersect( dirtyRect );
            painter->drawImage( rect, *viewParams->canvasImage(), rect );
        }
        else {
            painter->drawImage( dirtyRect, *viewParams->canvasImage(), dirtyRect );
        }
    }

//    qDebug( "Painted in %ims", t.elapsed() );

    renderPositions.clear();
    renderPositions << "SURFACE";

    // Paint the vector layer.
    if ( d->m_veccomposer && d->m_mapTheme->map()->hasVectorLayers() ) {

        if ( !d->m_mapTheme->map()->hasTextureLayers() ) {
            d->m_veccomposer->paintBaseVectorMap( painter, viewParams );
        }

        d->m_layerManager->renderLayers( painter, viewParams, renderPositions );

        // Add further Vectors
        d->m_veccomposer->paintVectorMap( painter, viewParams );
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

    if ( showPlaces && ( showCities || showTerrain || showOtherPlaces )
         && d->m_popSortModel->rowCount() > 0 )
    {
        d->m_placemarkLayout->paintPlaceFolder( painter, viewParams,
                                                d->m_popSortModel,
                                                d->m_placemarkselectionmodel );
    }

    // Paint the Gps Layer
//    d->m_gpsLayer->setVisible( viewParams->showGps() );
    //FIXME:We might just send ViewParams instead of this bunch of parameters
    d->m_gpsLayer->paintLayer( painter,
                               viewParams->canvasImage()->size(),
                               viewParams );

    renderPositions.clear();
    renderPositions << "HOVERS_ABOVE_SURFACE";
    d->m_layerManager->renderLayers( painter, viewParams, renderPositions );

    // FIXME: This is really slow. That's why we defer this to
    //        PrintQuality. Either cache on a pixmap - or maybe
    //        better: Add to GlobeScanlineTextureMapper.

    if ( viewParams->mapQuality() == PrintQuality )
        d->drawFog( painter, viewParams,
                    width, height );

    renderPositions.clear();
    renderPositions << "ATMOSPHERE"
                    << "ORBIT" << "ALWAYS_ON_TOP" << "FLOAT_ITEM" << "USER_TOOLS";
                               
    d->m_layerManager->renderLayers( painter, viewParams, renderPositions );
}


QAbstractItemModel *MarbleModel::placemarkModel() const
{
    return d->m_dataFacade->placemarkModel();
}

QItemSelectionModel *MarbleModel::placemarkSelectionModel() const
{
    return d->m_placemarkselectionmodel;
}

VectorComposer *MarbleModel::vectorComposer() const
{
    return d->m_veccomposer;
}

TextureColorizer *MarbleModel::textureColorizer() const
{
    return d->m_texcolorizer;
}

AbstractScanlineTextureMapper *MarbleModel::textureMapper() const
{
    return d->m_texmapper;
}

PlacemarkLayout *MarbleModel::placemarkLayout() const
{
    return d->m_placemarkLayout;
}

GpsLayer *MarbleModel::gpsLayer() const
{
    return d->m_gpsLayer;
}

GpxFileModel *MarbleModel::gpxFileModel() const
{
    return d->m_gpxFileModel;
}

FileViewModel *MarbleModel::fileViewModel() const
{
    return d->m_dataFacade->fileViewModel();
}

void MarbleModel::openGpxFile( const QString& filename )
{
    GpxFile* gpxFile = new GpxFile( filename );
    d->m_fileManager->addFile( filename );
    d->m_gpxFileModel->addFile( gpxFile );
}

void MarbleModel::addPlacemarkFile( const QString& filename )
{
    d->m_fileManager->addFile( filename );

    d->notifyModelChanged();
}

void MarbleModel::addPlacemarkData( const QString& data, const QString& key )
{
    d->m_fileManager->addData( key, data );

    d->notifyModelChanged();
}

void MarbleModel::removePlacemarkKey( const QString& fileName )
{
    d->m_fileManager->removeFile( fileName );

    d->notifyModelChanged();
}

QVector<QModelIndex> MarbleModel::whichFeatureAt( const QPoint& curpos ) const
{
    return d->m_placemarkLayout->whichPlacemarkAt( curpos );
}

void MarbleModelPrivate::notifyModelChanged()
{
    emit m_parent->modelChanged();
}

void MarbleModelPrivate::drawFog( QPainter *painter,
                                  ViewParams *viewParams,
                                  int width,
                                  int height )
{
    if ( viewParams->projection() != Spherical)
        return;

    // No use to draw the fog if it's not visible in the area.
    if ( viewParams->viewport()->mapCoversViewport() )
        return;

    int imgWidth2  = width / 2;
    int imgHeight2 = height / 2;

    int radius = viewParams->radius();

    // Recalculate the atmosphere effect and paint it to canvasImage.
    QRadialGradient grad1( QPointF( imgWidth2, imgHeight2 ), radius );

    // FIXME: Add a cosine relationship
    grad1.setColorAt( 0.85, QColor( 255, 255, 255, 0 ) );
    grad1.setColorAt( 1.00, QColor( 255, 255, 255, 64 ) );

    QBrush    brush1( grad1 );
    QPen      pen1( Qt::NoPen );

    painter->save();

    painter->setBrush( brush1 );
    painter->setPen( pen1 );
    painter->setRenderHint( QPainter::Antialiasing, false );

    // FIXME: Cut out what's really needed
    painter->drawEllipse( imgWidth2  - radius,
                         imgHeight2 - radius,
                         2 * radius,
                         2 * radius );

    painter->restore();
}

void MarbleModel::update()
{
    mDebug() << "MarbleModel::update()";
    QTimer::singleShot( 0, d->m_tileLoader, SLOT( update() ) );
}

qreal MarbleModel::planetRadius()   const
{
    return d->m_planet->radius();
}

QString MarbleModel::planetName()   const
{
    return d->m_planet->name();
}

ExtDateTime* MarbleModel::dateTime() const
{
//    mDebug() << "In dateTime, model:" << this;
//    mDebug() << d << ":" << d->m_dateTime;
    return d->m_dateTime;
}

SunLocator* MarbleModel::sunLocator() const
{
    return d->m_sunLocator;
}

MergedLayerDecorator* MarbleModel::layerDecorator() const
{
    return d->m_layerDecorator;
}

void MarbleModel::clearVolatileTileCache()
{
    d->m_tileLoader->update();
    mDebug() << "Cleared Volatile Cache!";
}

quint64 MarbleModel::volatileTileCacheLimit() const
{
    return d->m_tileLoader->volatileCacheLimit();
}

void MarbleModel::setVolatileTileCacheLimit( quint64 kiloBytes )
{
    d->m_tileLoader->setVolatileCacheLimit( kiloBytes );
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

void MarbleModel::paintTile( StackedTile* tile, GeoSceneTexture *textureLayer )
{
//    mDebug() << "MarbleModel::paintTile: " << "x: " << x << "y:" << y << "level: " << level
//             << "requestTileUpdate" << requestTileUpdate;

    d->m_layerDecorator->setInfo( tile->id() );
    d->m_layerDecorator->setTile( tile->resultTile() );

    d->m_layerDecorator->paint( "maps/" + textureLayer->sourceDir(), mapTheme() );
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

Planet* MarbleModel::planet() const
{
    return d->m_planet;
}

int MarbleModel::tileZoomLevel() const
{
    if (!d->m_texmapper)
        return -1;

    return d->m_texmapper->tileZoomLevel();
}

void MarbleModel::reloadMap() const
{
    Q_ASSERT( d->m_tileLoader );
    QList<TileId> displayed = d->m_tileLoader->tilesOnDisplay();
    QList<TileId>::const_iterator pos = displayed.constBegin();
    QList<TileId>::const_iterator const end = displayed.constEnd();
    for (; pos != end; ++pos ) {
        // it's debatable here, whether DownloadBulk or DownloadBrowse should be used
        // but since "reload" or "refresh" seems to be a common action of a browser and it
        // allows for more connections (in our model), use "DownloadBrowse"
        d->m_tileLoader->reloadTile( *pos, DownloadBrowse );
    }
}

void MarbleModel::downloadRegion( QString const & mapThemeId,
                                  TileCoordsPyramid const & pyramid ) const
{
    Q_ASSERT( d->m_tileLoader );
    QTime t;
    t.start();
    // When downloading a region (the author of these lines thinks) most users probably expect
    // the download to begin with the low resolution tiles and then procede level-wise to
    // higher resolution tiles. In order to achieve this, we start requesting downloads of
    // high resolution tiles and request the low resolution tiles at the end because
    // DownloadQueueSet (silly name) is implemented as stack.
    for ( int level = pyramid.bottomLevel(); level >= pyramid.topLevel(); --level ) {
        QRect const coords = pyramid.coords( level );
        mDebug() << "MarbleModel::downloadRegion level:" << level << "tile coords:" << coords;
        int x1, y1, x2, y2;
        coords.getCoords( &x1, &y1, &x2, &y2 );
        for ( int x = x1; x <= x2; ++x )
            for ( int y = y1; y <= y2; ++y ) {
                TileId const tileId( mapThemeId, level, x, y );
                // FIXME: use lazy evaluation to not generate up to 100k tiles in one go
                // this can take considerable time even on very fast systems
                // in contrast generating the TileIds on the fly when they are needed
                // does not seem to affect download speed.
                d->m_tileLoader->downloadTile( tileId );
            }
    }
    int const elapsedMs = t.elapsed();
    mDebug() << "MarbleModel::downloadRegion:" << pyramid.tilesCount() << "tiles, " << elapsedMs << "ms";
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
GeoSceneTexture * MarbleModel::textureLayer() const
{
    if ( !d->m_mapTheme )
        return 0;
    if ( !d->m_mapTheme->map()->hasTextureLayers() )
        return 0;

    // As long as we don't have an Layer Management Class we just lookup
    // the name of the layer that has the same name as the theme Id
    const QString themeId = d->m_mapTheme->head()->theme();
    GeoSceneLayer * const layer = static_cast<GeoSceneLayer*>( d->m_mapTheme->map()->layer( themeId ));
    if ( !layer )
        return 0;

    return static_cast<GeoSceneTexture*>( layer->groundDataset() );
}

}

#include "MarbleModel.moc"
