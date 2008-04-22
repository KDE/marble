//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "MarbleModel.h"

#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtGui/QSortFilterProxyModel>

#include "global.h"
#include "gps/GpsLayer.h"

#include "GeoSceneDocument.h"
#include "GeoSceneLayer.h"
#include "GeoSceneTexture.h"
#include "GeoSceneVector.h"

#include "AbstractScanlineTextureMapper.h"
#include "ClipPainter.h"
#include "FileViewModel.h"
#include "FlatScanlineTextureMapper.h"
#include "GeoPolygon.h"
#include "GlobeScanlineTextureMapper.h"
#include "GridMap.h"
#include "HttpDownloadManager.h"
#include "KMLFileViewItem.h"
#include "MapThemeManager.h"
#include "MarbleDirs.h"
#include "MarblePlacemarkModel.h"
#include "VectorComposer.h"
#include "ViewParams.h"
#include "SunLocator.h"
#include "TextureColorizer.h"
#include "TileCreator.h"
#include "TileCreatorDialog.h"
#include "TileLoader.h"
#include "PlaceMarkManager.h"
#include "PlaceMarkLayout.h"
#include "PlaceMarkPainter.h"
#include "XmlHandler.h"
#include "TextureTile.h"


class MarbleModelPrivate
{
 public:
    MarbleModelPrivate( MarbleModel *parent )
        : m_parent( parent ),
          m_mapTheme( 0 )
    {
    }

    void  resize( int width, int height );
    void  notifyModelChanged();
    void  geoDataDocumentLoaded( GeoDataDocument& document );

    MarbleModel         *m_parent;

    // View and paint stuff
    GeoSceneDocument    *m_mapTheme;
    QString              m_selectedMap;
    bool                 m_previousMapLoadedFine;
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
    d->m_timer = new QTimer( this );
    d->m_timer->start( 200 );

    connect( d->m_timer, SIGNAL( timeout() ),
             this,       SIGNAL( timeout() ) );

    d->m_downloadManager = 0;

    d->m_tileLoader = new TileLoader( d->m_downloadManager, this );

    qDebug() << "d->m_tileLoader =" << d->m_tileLoader;
    connect( d->m_tileLoader, SIGNAL( paintTile(TextureTile*, int, int, int, const QString&, bool) ),
             this,            SLOT( paintTile(TextureTile*, int, int, int, const QString&, bool) ) );

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

    connect( d->m_placemarkselectionmodel, SIGNAL( selectionChanged ( QItemSelection, QItemSelection) ),
             d->m_placeMarkLayout,         SLOT( requestStyleReset() ) ); 
    connect( d->m_placemarkmodel, SIGNAL( modelReset() ),
             d->m_placeMarkLayout,         SLOT( requestStyleReset() ) ); 

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
    
    m_sunLocator = new SunLocator();
    m_layerDecorator = new MergedLayerDecorator(m_sunLocator);

    connect( m_layerDecorator, SIGNAL( repaintMap() ), SIGNAL( modelChanged() ) );

    // TODO be able to set these somewhere
    m_layerDecorator->setShowClouds(true);
    m_layerDecorator->setShowTileId(false);
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
//    delete d->m_maptheme;
    delete d->m_mapTheme;
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

QString MarbleModel::mapTheme() const
{
    return d->m_selectedMap;
}

GeoSceneDocument* MarbleModel::mapThemeObject() const
{
    return d->m_mapTheme;
}

// Set a particular theme for the map and load the appropriate tile level.
// If the tiles (for the lowest tile level) haven't been created already
// then create them here and now.
//
// FIXME: Get rid of that awful 'parent' parameter and move the tile
//        creation dialogs out of this function.  Change them into signals 
//        instead.
// FIXME: Get rid of 'currentProjection' here.  It's totally misplaced.
//

void MarbleModel::setMapTheme( const QString &selectedMap, QObject *parent,
			       Projection currentProjection )
{
    // Here we start refactoring the map theme
    GeoSceneDocument* mapTheme = MapThemeManager::loadMapTheme( selectedMap ); // Hardcoded for a start

    // Check whether the selected theme got parsed well
    if ( !mapTheme ) {

        // Check whether the previous theme works 
        if ( !d->m_mapTheme ){ 
            // Fall back to default theme
            qDebug() << "Falling back to default theme";
            mapTheme = MapThemeManager::loadMapTheme("earth/srtm/srtm.dgml");

            // If this last resort doesn't work either shed a tear and exit
            if ( !mapTheme ) {
                qDebug() << "Couldn't find a valid DGML map.";
                exit(-1);
            }
        }
        else {
            qDebug() << "Selected theme doesn't work, so we stick to the previous one";
            return;
        }
    }

    d->m_mapTheme = mapTheme;

    // Some output to show how to use this stuff ...
    qDebug() << "DGML2 Name       : " << d->m_mapTheme->head()->name(); 
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

        if ( !TileLoader::baseTilesAvailable( "maps/"
                                              + sourceDir ) )
        {
            qDebug("DGML2: Base tiles not available. Creating Tiles ... ");

            TileCreator *tileCreator = new TileCreator(
                                     sourceDir,
                                     installMap,
                                     (role == "dem") ? "true" : "false" );
            // FIXME
            // TileCreatorDialog tileCreatorDlg( tileCreator, parent );
            TileCreatorDialog tileCreatorDlg( tileCreator, 0 );
                tileCreatorDlg.setSummary( d->m_mapTheme->head()->name(),
                                           d->m_mapTheme->head()->description() );
                tileCreatorDlg.exec();
                qDebug("Tile creation completed");
        }

        delete d->m_texmapper;

        d->m_tileLoader->setMapTheme( "maps/" + sourceDir );

        switch( currentProjection ) {
            case Spherical:
                d->m_texmapper = new GlobeScanlineTextureMapper( d->m_tileLoader, this );
                break;
            case Equirectangular:
                d->m_texmapper = new FlatScanlineTextureMapper( d->m_tileLoader, this );
                break;
            case Mercator:
                d->m_texmapper = new FlatScanlineTextureMapper( d->m_tileLoader, this );
                break;

        }

        connect( d->m_texmapper, SIGNAL( mapChanged() ),
                 this,           SLOT( notifyModelChanged() ) );
    }
    else {
        d->m_tileLoader->flush();
    }

    // Set all the colors for the vector layers
    if ( d->m_mapTheme->map()->hasVectorLayers() ) {
        d->m_veccomposer->setOceanColor( d->m_mapTheme->map()->backgroundColor() );

        // Just as with textures this is a workaround for DGML2 to emulate the old behaviour

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
                d->m_veccomposer->setLandColor( vector->brush().color() );
        }
    }

    if ( d->m_placeMarkLayout == 0)
        d->m_placeMarkLayout = new PlaceMarkLayout( this );
    d->m_placeMarkLayout->requestStyleReset();
    // FIXME: To be removed after MapTheme / KML refactoring

    // FIXME: Still needs to get fixed for the DGML2 refactoring
//    d->m_placeMarkLayout->placeMarkPainter()->setDefaultLabelColor( d->m_maptheme->labelColor() );

    d->m_selectedMap = selectedMap;
    emit themeChanged( selectedMap );
    d->notifyModelChanged();
}


int MarbleModel::minimumZoom() const
{
    if ( d->m_mapTheme )
        return d->m_mapTheme->head()->zoom()->minimum();
    return 0;
}

int MarbleModel::maximumZoom() const
{
    if ( d->m_mapTheme )
        return d->m_mapTheme->head()->zoom()->maximum();
    return 0;
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


void MarbleModel::paintGlobe( ClipPainter* painter,
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

            d->m_texmapper->mapTexture( viewParams );

            if ( !viewParams->m_showElevationModel
                && layer->role() == "dem" )
            {
                viewParams->m_coastImage->fill( Qt::transparent );

                // Create VectorMap
                d->m_veccomposer->drawTextureMap( viewParams );

                // Recolorize the heightmap using the VectorMap
                d->m_texcolorizer->colorize( viewParams );
            }
        }
    }
    // Paint the map on the Widget
//    QTime t;
//    t.start();
    int radius = (int)(1.05 * (double)(viewParams->radius()));

    if ( d->m_mapTheme->map()->hasTextureLayers() ) {
        if ( viewParams->projection() == Spherical ) {
            QRect rect( width / 2 - radius , height / 2 - radius, 2 * radius, 2 * radius);
            rect = rect.intersect( dirtyRect );
            painter->drawImage( rect, *viewParams->m_canvasImage, rect );
        }
        else {
            painter->drawImage( dirtyRect, *viewParams->m_canvasImage, dirtyRect );
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
    if ( viewParams->m_showGrid ) {
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
    if ( viewParams->m_showPlaceMarks 
         && ( viewParams->m_showCities
              || viewParams->m_showTerrain 
              || viewParams->m_showOtherPlaces )
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
    d->m_gpsLayer->setVisible( viewParams->m_showGps );
    //FIXME:We might just send ViewParams instead of this bunch of parameters
    d->m_gpsLayer->paintLayer( painter,
                               viewParams->m_canvasImage->size(),
                               viewParams,
                               viewParams->m_boundingBox );
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
    AbstractFileViewItem* item = new KMLFileViewItem( *m_placemarkmanager,
                                                      document );

    m_fileviewmodel->append( item );
}

void MarbleModel::update()
{
    QTimer::singleShot( 0, d->m_tileLoader, SLOT( update() ) );
}

SunLocator* MarbleModel::sunLocator() const
{
    return m_sunLocator;
}

MergedLayerDecorator* MarbleModel::layerDecorator() const
{
    return m_layerDecorator;
}

quint64 MarbleModel::volatileTileCacheLimit() const
{
    return d->m_tileLoader->volatileCacheLimit();
}

void MarbleModel::setVolatileTileCacheLimit( quint64 kiloBytes )
{
    d->m_tileLoader->setVolatileCacheLimit( kiloBytes );
}

void MarbleModel::paintTile(TextureTile* tile, int x, int y, int level, const QString& theme, bool requestTileUpdate)
{
    qDebug() << "MarbleModel::paintTile";
    
    if ( d->m_downloadManager != 0 ) {
        connect( m_layerDecorator, SIGNAL( downloadTile( const QString&, const QString& ) ),
                 d->m_downloadManager, SLOT( addJob( const QString&, const QString& ) ) );
    }

    m_layerDecorator->setInfo(x, y, level, tile->id());
    m_layerDecorator->setTile(tile->tile());
    m_layerDecorator->paint(theme);
    tile->loadTile(requestTileUpdate);
}

#include "MarbleModel.moc"
