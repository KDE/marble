//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "MarbleModel.h"

#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtGui/QItemSelectionModel>
#include <QtGui/QSortFilterProxyModel>

#include "global.h"
#include "gps/GpsLayer.h"

#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneLayer.h"
#include "GeoSceneMap.h"
#include "GeoSceneTexture.h"
#include "GeoSceneVector.h"

#include "GeoPainter.h"
#include "FileViewModel.h"
#include "SphericalScanlineTextureMapper.h"
#include "EquirectScanlineTextureMapper.h"
#include "MercatorScanlineTextureMapper.h"
#include "GeoPolygon.h"
#include "gps/GpxFileModel.h"
#include "GridMap.h"
#include "HttpDownloadManager.h"
#include "KmlFileViewItem.h"
#include "LayerManager.h"
#include "MapThemeManager.h"
#include "MarbleDataFacade.h"
#include "MarbleDirs.h"
#include "MarblePlacemarkModel.h"
#include "PlaceMarkManager.h"
#include "PlaceMarkLayout.h"
#include "PlaceMarkPainter.h"
#include "StoragePolicy.h"
#include "SunLocator.h"
#include "TextureColorizer.h"
#include "TextureTile.h"
#include "TileCreator.h"
#include "TileCreatorDialog.h"
#include "TileLoader.h"
#include "VectorComposer.h"
#include "ViewParams.h"
#include "ViewportParams.h"


class MarbleModelPrivate
{
 public:
    MarbleModelPrivate( MarbleModel *parent )
        : m_parent( parent ),
          m_dataFacade( 0 ),
          m_mapTheme( 0 ),
          m_layerManager( 0 )
    {
    }

    void  resize( int width, int height );
    void  notifyModelChanged();
    void  geoDataDocumentLoaded( GeoDataDocument& document );

    MarbleModel         *m_parent;
    MarbleDataFacade    *m_dataFacade;

    // View and paint stuff
    GeoSceneDocument    *m_mapTheme;
    LayerManager        *m_layerManager;
    TextureColorizer    *m_texcolorizer;

    HttpDownloadManager *m_downloadManager;

    TileLoader          *m_tileLoader;
    AbstractScanlineTextureMapper   *m_texmapper;

    VectorComposer      *m_veccomposer; // FIXME: Make not a pointer.
    GridMap             *m_gridmap;

    // Places on the map
    PlaceMarkManager      *m_placemarkmanager;
    MarblePlacemarkModel  *m_placemarkmodel;
    PlaceMarkLayout       *m_placeMarkLayout;


    // Misc stuff
    ExtDateTime           *m_dateTime;
    SunLocator            *m_sunLocator;
    MergedLayerDecorator  *m_layerDecorator;

    // Selection handling
    QItemSelectionModel *m_placemarkselectionmodel;

    //Gps Stuff
    GpsLayer            *m_gpsLayer;
    GpxFileModel        *m_gpxFileModel;

    QTimer       *m_timer;

    FileViewModel       *m_fileviewmodel;
};

MarbleModel::MarbleModel( QObject *parent )
    : QObject( parent ),
      d( new MarbleModelPrivate( this ) )
{
    d->m_dataFacade = new MarbleDataFacade( this );
    d->m_layerManager = new LayerManager( d->m_dataFacade );

    // FIXME: more on the spot update names and API
    connect ( d->m_layerManager,      SIGNAL( floatItemsChanged() ),
              this,                   SIGNAL( modelChanged() ) );

    d->m_timer = new QTimer( this );
    d->m_timer->start( 200 );

    connect( d->m_timer, SIGNAL( timeout() ),
             this,       SIGNAL( timeout() ) );

    d->m_downloadManager = 0;

    d->m_tileLoader = new TileLoader( d->m_downloadManager, this );

    connect( d->m_tileLoader, SIGNAL( paintTile(TextureTile*, int, int, int, GeoSceneTexture*, bool) ),
             this,            SLOT( paintTile(TextureTile*, int, int, int, GeoSceneTexture*, bool) ) );

    d->m_texmapper = 0;
    d->m_veccomposer = new VectorComposer();

    d->m_placeMarkLayout   = 0;

    d->m_gridmap      = new GridMap();
    d->m_texcolorizer = new TextureColorizer( MarbleDirs::path( "seacolors.leg" ),
                                              MarbleDirs::path( "landcolors.leg" ) );

    d->m_placemarkmanager   = new PlaceMarkManager();

    connect( d->m_placemarkmanager, SIGNAL( geoDataDocumentLoaded( GeoDataDocument& ) ),
             this,                  SLOT( geoDataDocumentLoaded( GeoDataDocument& ) ) );

    d->m_placemarkmodel = new MarblePlacemarkModel( d->m_placemarkmanager, this );
    d->m_placemarkselectionmodel = new QItemSelectionModel( d->m_placemarkmodel );

    d->m_placemarkmanager->loadStandardPlaceMarks();

    d->m_gpxFileModel = new GpxFileModel( this );
    d->m_gpsLayer = new GpsLayer( d->m_gpxFileModel );

    connect( d->m_gpxFileModel, SIGNAL( updateRegion( BoundingBox& ) ),
             this,              SIGNAL( regionChanged( BoundingBox& ) ) );

    /*
     * Create FileViewModel
     */
    d->m_fileviewmodel = new FileViewModel( this );

    connect( d->m_fileviewmodel, SIGNAL( updateRegion( BoundingBox& ) ),
             this,               SIGNAL( regionChanged( BoundingBox& ) ) );

    d->m_dateTime       = new ExtDateTime();
    d->m_sunLocator     = new SunLocator( d->m_dateTime );
    d->m_layerDecorator = new MergedLayerDecorator( d->m_sunLocator );

    connect(d->m_dateTime,   SIGNAL( timeChanged() ),
            d->m_sunLocator, SLOT( update() ) );
    connect( d->m_layerDecorator, SIGNAL( repaintMap() ),
                                  SIGNAL( modelChanged() ) );

    // TODO be able to set these somewhere
    d->m_layerDecorator->setShowClouds( true );
    d->m_layerDecorator->setShowTileId( false );
}

MarbleModel::~MarbleModel()
{
    qDebug() << "MarbleModel::~MarbleModel";
    
    delete d->m_texmapper;

    delete d->m_tileLoader; // disconnects from downloadManager in dtor
    delete d->m_downloadManager;

    delete d->m_veccomposer;
    delete d->m_texcolorizer; 
    delete d->m_gridmap;
    delete d->m_placemarkmodel;
    delete d->m_placemarkmanager;
    delete d->m_gpsLayer;
    delete d->m_mapTheme;
    delete d->m_timer;
    delete d->m_layerManager;
    delete d->m_dataFacade;
    delete d;
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

    // Some output to show how to use this stuff ...
    qDebug() << "DGML2 Name       : " << d->m_mapTheme->head()->name(); 
/*
    qDebug() << "DGML2 Description: " << d->m_mapTheme->head()->description(); 

    if ( d->m_mapTheme->map()->hasTextureLayers() )
        qDebug() << "Contains texture layers! ";
    else
        qDebug() << "Does not contain any texture layers! ";

    qDebug() << "Number of SRTM textures: " << d->m_mapTheme->map()->layer("srtm")->datasets().count();

    if ( d->m_mapTheme->map()->hasVectorLayers() )
        qDebug() << "Contains vector layers! ";
    else
        qDebug() << "Does not contain any vector layers! ";
*/

    if ( d->m_mapTheme->map()->hasTextureLayers() ) {
        // If the tiles aren't already there, put up a progress dialog
        // while creating them.

        // As long as we don't have an Layer Management Class we just lookup 
        // the name of the layer that has the same name as the theme ID
        QString themeID = d->m_mapTheme->head()->theme();

        GeoSceneTexture *texture = 
            static_cast<GeoSceneTexture*>( d->m_mapTheme->map()->layer( themeID )->datasets().first() );

        QString sourceDir = texture->sourceDir();
        QString installMap = texture->installMap();
        QString role = d->m_mapTheme->map()->layer( themeID )->role();

        if ( !TileLoader::baseTilesAvailable( texture )
            && !installMap.isEmpty() )
        {
            qDebug() << "Base tiles not available. Creating Tiles ... \n"
                     << "SourceDir: " << sourceDir << "InstallMap:" << installMap;
            MarbleDirs::debug();

            TileCreator *tileCreator = new TileCreator(
                                     sourceDir,
                                     installMap,
                                     (role == "dem") ? "true" : "false" );

            TileCreatorDialog  tileCreatorDlg( tileCreator, 0 );
	    tileCreatorDlg.setSummary( d->m_mapTheme->head()->name(),
				       d->m_mapTheme->head()->description() );
	    tileCreatorDlg.exec();
	    qDebug("Tile creation completed");
        }
        d->m_tileLoader->setTextureLayer( texture );
    }
    else {
        d->m_tileLoader->flush();
    }

    setupTextureMapper( currentProjection );

    // Set all the colors for the vector layers
    if ( d->m_mapTheme->map()->hasVectorLayers() ) {
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

    if ( d->m_placeMarkLayout == 0) {
        d->m_placeMarkLayout = new PlaceMarkLayout( this );
        connect( d->m_placemarkselectionmodel, SIGNAL( selectionChanged( QItemSelection,
                                                                         QItemSelection) ),
                 d->m_placeMarkLayout, SLOT( requestStyleReset() ) );
        connect( d->m_placemarkmodel, SIGNAL( modelReset() ),
                 d->m_placeMarkLayout, SLOT( requestStyleReset() ) );
    }

    d->m_placeMarkLayout->requestStyleReset();
    // FIXME: To be removed after MapTheme / KML refactoring

    // FIXME: Still needs to get fixed for the DGML2 refactoring
//    d->m_placeMarkLayout->placeMarkPainter()->setDefaultLabelColor( d->m_maptheme->labelColor() );

    if( !d->m_mapTheme->map()->filters().isEmpty() ) {
        GeoSceneFilter *filter= d->m_mapTheme->map()->filters().first();
        QString seafile = MarbleDirs::path( "seacolors.leg" );
        QString landfile = MarbleDirs::path( "landcolors.leg" );

        if( filter->type() == "colorize" ) {
            QList<GeoScenePalette*> palette = filter->palette();
            foreach ( GeoScenePalette *curPalette, palette ) {
                if( curPalette->type() == "sea" ) {
                    seafile = MarbleDirs::path( curPalette->file() );
                } else if( curPalette->type() == "land" ) {
                    landfile = MarbleDirs::path( curPalette->file() );
                }
            }
        }
        if( d->m_texcolorizer->seafile() != seafile ||
            d->m_texcolorizer->landfile() != landfile ) {
            d->m_texcolorizer->generatePalette( seafile, landfile );
        }
    }
    qDebug() << "THEME CHANGED: ***" << mapTheme->head()->mapThemeId();
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
            d->m_texmapper = new SphericalScanlineTextureMapper( d->m_tileLoader, this );
            break;
        case Equirectangular:
            d->m_texmapper = new EquirectScanlineTextureMapper( d->m_tileLoader, this );
            break;
        case Mercator:
            d->m_texmapper = new MercatorScanlineTextureMapper( d->m_tileLoader, this );
            break;
    }

    connect( d->m_texmapper, SIGNAL( mapChanged() ),
	     this,           SLOT( notifyModelChanged() ) );
}

HttpDownloadManager* MarbleModel::downloadManager() const
{
    return d->m_downloadManager;
}


void MarbleModel::setDownloadManager( HttpDownloadManager *downloadManager )
{
    HttpDownloadManager *previousDownloadManager = d->m_downloadManager;

    d->m_tileLoader->setDownloadManager( downloadManager );
    d->m_downloadManager = downloadManager;

    delete previousDownloadManager;
}


void MarbleModelPrivate::resize( int width, int height )
{
    if ( m_mapTheme->map()->hasTextureLayers() ) {
        m_texmapper->resizeMap( width, height );
    }
    m_veccomposer->resizeMap( width, height );
}


void MarbleModel::paintGlobe( GeoPainter *painter,
                              int width, int height,
                              ViewParams *viewParams,
                              bool redrawBackground,
                              const QRect& dirtyRect )
{
    d->resize( width, height );

    // FIXME: Remove this once the LMC is there:
    QString themeID = d->m_mapTheme->head()->theme();

    GeoSceneLayer *layer = 
        static_cast<GeoSceneLayer*>( d->m_mapTheme->map()->layer( themeID ) );

    if ( redrawBackground ) {
        if ( d->m_mapTheme->map()->hasTextureLayers() ) {

	    // Create the height map image a.k.a viewParams->m_canvasImage.
            d->m_texmapper->mapTexture( viewParams );

            if ( !viewParams->showElevationModel()
                && layer->role() == "dem"
                && !d->m_mapTheme->map()->filters().isEmpty() ) {

                GeoSceneFilter *filter= d->m_mapTheme->map()->filters().first();
                viewParams->coastImage()->fill( Qt::transparent );
                // Create VectorMap
                d->m_veccomposer->drawTextureMap( viewParams );

                //set default values just in case
                QString seafile = MarbleDirs::path( "seacolors.leg" );
                QString landfile = MarbleDirs::path( "landcolors.leg" );

                if( filter->type() == "colorize" ) {
                    QList<GeoScenePalette*> palette = filter->palette();
                    foreach ( GeoScenePalette *curPalette, palette ) {
                        if( curPalette->type() == "sea" ) {
                            seafile = MarbleDirs::path( curPalette->file() );
                        } else if( curPalette->type() == "land" ) {
                            landfile = MarbleDirs::path( curPalette->file() );
                        }
                    }
                }
                if( d->m_texcolorizer->seafile() != seafile ||
                    d->m_texcolorizer->landfile() != landfile ) {

                    d->m_texcolorizer->generatePalette( seafile, landfile );
                }
                // Recolorize the heightmap using the VectorMap
                d->m_texcolorizer->colorize( viewParams );
            } //else { qDebug() << "No filters to act on..."; }
        }
    }

    // Paint the map on the Widget
//    QTime t;
//    t.start();
    int radius = (int)(1.05 * (double)(viewParams->radius()));

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

    // Paint the vector layer.
    if ( d->m_mapTheme->map()->hasVectorLayers() ) {

        if ( !d->m_mapTheme->map()->hasTextureLayers() ) {
            d->m_veccomposer->paintBaseVectorMap( painter, viewParams );
        }
        // Add further Vectors
        d->m_veccomposer->paintVectorMap( painter, viewParams );
    }

    // Paint the lon/lat grid around the earth.
    bool showGrid;

    viewParams->propertyValue( "coordinate-grid", showGrid );

    if ( showGrid ) {
        QPen  gridpen( QColor( 231, 231, 231, 255 ) );

        // FIXME: Why would the createFoo() functions be exposed in
        //        the class GridMap?  Shouldn't just a call to
        //        paintGrid() or paintEquator be enough?  The internal
        //        create...() functions should be private and not
        //        exposed to the outside.

        // Create and paint a grid
        d->m_gridmap->createGrid( viewParams->viewport() );
        d->m_gridmap->setPen( gridpen );

        bool antialiased = false;

        if (   viewParams->mapQuality() == Marble::High
            || viewParams->mapQuality() == Marble::Print ) {
                antialiased = true;
        }

        d->m_gridmap->paintGridMap( painter, antialiased );

        // Create and paint the tropics and polar circles
        d->m_gridmap->createTropics( viewParams->viewport() );
        gridpen.setStyle( Qt::DotLine );
        // gridpen.setWidthF( 1.5f );
        d->m_gridmap->setPen( gridpen );
        d->m_gridmap->paintGridMap( painter, antialiased );

        // Create and paint Equator
        d->m_gridmap->createEquator( viewParams->viewport() );
        // gridpen.setWidthF( 2.0f );
        d->m_gridmap->setPen( gridpen );
        d->m_gridmap->paintGridMap( painter, antialiased );
    }

    // Paint the GeoDataPlaceMark layer
#ifndef KML_GSOC
    bool showPlaces, showCities, showTerrain, showOtherPlaces;

    viewParams->propertyValue( "places", showPlaces );
    viewParams->propertyValue( "cities", showCities );
    viewParams->propertyValue( "terrain", showTerrain );
    viewParams->propertyValue( "otherplaces", showOtherPlaces );

    if ( showPlaces && ( showCities || showTerrain || showOtherPlaces )
         && d->m_placemarkmodel->rowCount() > 0 )
    {
        d->m_placeMarkLayout->paintPlaceFolder( painter, viewParams,
                                                d->m_placemarkmodel,
                                                d->m_placemarkselectionmodel );
    }
#else
    /*
     * Iterate over our KMLFolders and show placemarks from each folderList
     * This folder list also will be displayed in MarbleControlBox
     * So user could enable/disable each folder
     */
    if ( viewParams->m_showPlaceMarks ) {
        QTime t;
        t.start ();

        const QList < GeoDataFolder* >& folderList = d->m_placemarkmanager->getFolderList();

        bool firstTime = true;

        for ( QList<GeoDataFolder*>::const_iterator iterator = folderList.constBegin();
            iterator != folderList.constEnd();
            ++iterator )
        {
            GeoDataFolder& folder = *( *iterator );

            /*
             * Show only placemarks which are visible
             */
            if ( folder.isVisible() ) {

                PlaceMarkContainer& container = folder.activePlaceMarkContainer( *viewParams );

                d->m_placeMarkLayout->paintPlaceFolder( painter, viewParams,
                                                        &container, firstTime );

                firstTime = false;
            }
        }

        qDebug("Paint kml folder list. Elapsed: %d", t.elapsed());
    }
#endif

    // Paint the Gps Layer
    d->m_gpsLayer->setVisible( viewParams->showGps() );
    //FIXME:We might just send ViewParams instead of this bunch of parameters
    d->m_gpsLayer->paintLayer( painter,
                               viewParams->canvasImage()->size(),
                               viewParams,
                               viewParams->viewport()->boundingBox() );

    d->m_layerManager->renderLayers( painter, viewParams );
}


QAbstractItemModel *MarbleModel::placeMarkModel() const
{
    return d->m_placemarkmodel;
}

QItemSelectionModel *MarbleModel::placeMarkSelectionModel() const
{
    return d->m_placemarkselectionmodel;
}

VectorComposer    *MarbleModel::vectorComposer()   const
{
    return d->m_veccomposer;
}

TextureColorizer  *MarbleModel::textureColorizer() const
{
    return d->m_texcolorizer;
}

AbstractScanlineTextureMapper    *MarbleModel::textureMapper()    const
{
    return d->m_texmapper;
}

PlaceMarkLayout *MarbleModel::placeMarkLayout() const
{
    return d->m_placeMarkLayout;
}

GpsLayer *MarbleModel::gpsLayer()         const
{
    return d->m_gpsLayer;
}

GpxFileModel *MarbleModel::gpxFileModel()   const
{
    return d->m_gpxFileModel;
}

FileViewModel *MarbleModel::fileViewModel() const
{
    return d->m_fileviewmodel;
}

void MarbleModel::addPlaceMarkFile( const QString& filename )
{
    d->m_placemarkmanager->loadKml( filename, true );

    d->notifyModelChanged();
}

void MarbleModel::addPlaceMarkData( const QString& data )
{
    d->m_placemarkmanager->loadKmlFromData( data, true );

    d->notifyModelChanged();
}

QVector<QPersistentModelIndex> MarbleModel::whichFeatureAt( const QPoint& curpos ) const
{
    return d->m_placeMarkLayout->whichPlaceMarkAt( curpos );
}

void MarbleModelPrivate::notifyModelChanged()
{
    emit m_parent->modelChanged();
}

void MarbleModelPrivate::geoDataDocumentLoaded( GeoDataDocument& document )
{
    AbstractFileViewItem* item = new KmlFileViewItem( *m_placemarkmanager,
                                                      document );

    m_fileviewmodel->append( item );
}

void MarbleModel::update()
{
    QTimer::singleShot( 0, d->m_tileLoader, SLOT( update() ) );
}

ExtDateTime* MarbleModel::dateTime() const
{
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
    qDebug() << "Cleared Volatile Cache!";
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

        GeoSceneTexture *texture = 
            static_cast<GeoSceneTexture*>( d->m_mapTheme->map()->layer( themeID )->datasets().first() );

        QString sourceDir = texture->sourceDir();
        QString installMap = texture->installMap();
        QString role = d->m_mapTheme->map()->layer( themeID )->role();

        if ( !TileLoader::baseTilesAvailable( texture )
            && !installMap.isEmpty() )
        {
            qDebug() << "Base tiles not available. Creating Tiles ... \n"
                     << "SourceDir: " << sourceDir << "InstallMap:" << installMap;
            MarbleDirs::debug();

            TileCreator *tileCreator = new TileCreator(
                                     sourceDir,
                                     installMap,
                                     (role == "dem") ? "true" : "false" );

            TileCreatorDialog  tileCreatorDlg( tileCreator, 0 );
            tileCreatorDlg.setSummary( d->m_mapTheme->head()->name(),
                       d->m_mapTheme->head()->description() );
            tileCreatorDlg.exec();
            qDebug("Tile creation completed");
        }
    }
}

void MarbleModel::paintTile(TextureTile* tile, int x, int y, int level,
                            GeoSceneTexture *textureLayer, bool requestTileUpdate)
{
    qDebug() << "MarbleModel::paintTile: " << "x: " << x << "y:" << y << "level: " << level << "requestTileUpdate" << requestTileUpdate;
    
    if ( d->m_downloadManager != 0 ) {
        connect( d->m_layerDecorator, SIGNAL( downloadTile( QUrl, QString, QString ) ),
                 d->m_downloadManager, SLOT( addJob( QUrl, QString, QString ) ) );
    }

    d->m_layerDecorator->setInfo(x, y, level, tile->id());
    d->m_layerDecorator->setTile(tile->tile());
        
    d->m_layerDecorator->paint("maps/" + textureLayer->sourceDir(), mapTheme() );
    tile->loadTile(requestTileUpdate);
}

QList<MarbleAbstractLayer *> MarbleModel::layerPlugins() const
{
    return d->m_layerManager->layerPlugins();
}

QList<MarbleAbstractFloatItem *> MarbleModel::floatItems() const
{
    return d->m_layerManager->floatItems();
}

#include "MarbleModel.moc"
