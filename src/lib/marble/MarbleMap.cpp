//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2009 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Carlos Licea <carlos.licea@kdemail.net>
// Copyright 2009      Jens-Michael Hoffmann <jensmh@gmx.de>
// Copyright 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


// Own
#include "MarbleMap.h"

// Posix
#include <cmath>

// Qt
#include <QAbstractItemModel>
#include <QTime>
#include <QTimer>
#include <QItemSelectionModel>
#include <QSizePolicy>
#include <QRegion>

#ifdef MARBLE_DBUS
#include <QDBusConnection>
#endif

// Marble
#include "layers/FogLayer.h"
#include "layers/FpsLayer.h"
#include "layers/GeometryLayer.h"
#include "layers/GroundLayer.h"
#include "layers/MarbleSplashLayer.h"
#include "layers/PlacemarkLayer.h"
#include "layers/TextureLayer.h"
#include "layers/VectorTileLayer.h"
#include "AbstractFloatItem.h"
#include "DgmlAuxillaryDictionary.h"
#include "FileManager.h"
#include "GeoDataTreeModel.h"
#include "GeoPainter.h"
#include "GeoSceneDocument.h"
#include "GeoSceneFilter.h"
#include "GeoSceneGeodata.h"
#include "GeoSceneHead.h"
#include "GeoSceneLayer.h"
#include "GeoSceneMap.h"
#include "GeoScenePalette.h"
#include "GeoSceneSettings.h"
#include "GeoSceneVector.h"
#include "GeoSceneVectorTile.h"
#include "GeoSceneZoom.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataFeature.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "LayerManager.h"
#include "MapThemeManager.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "RenderPlugin.h"
#include "SunLocator.h"
#include "TileCoordsPyramid.h"
#include "TileCreator.h"
#include "TileCreatorDialog.h"
#include "TileLoader.h"
#include "ViewParams.h"
#include "ViewportParams.h"


namespace Marble
{


class MarbleMap::CustomPaintLayer : public LayerInterface
{
public:
    CustomPaintLayer( MarbleMap *map )
        : m_map( map )
    {
    }

    virtual QStringList renderPosition() const { return QStringList() << "USER_TOOLS"; }

    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
                         const QString &renderPos, GeoSceneLayer *layer )
    {
        Q_UNUSED( viewport );
        Q_UNUSED( renderPos );
        Q_UNUSED( layer );

        m_map->customPaint( painter );

        return true;
    }

    virtual qreal zValue() const { return 1.0e6; }

    RenderState renderState() const { return RenderState( "Custom Map Paint" ); }

    virtual QString runtimeTrace() const { return "CustomPaint"; }

private:
    MarbleMap *const m_map;
};


class MarbleMapPrivate
{
    friend class MarbleWidget;

public:
    explicit MarbleMapPrivate( MarbleMap *parent, MarbleModel *model );

    void updateMapTheme();

    void updateProperty( const QString &, bool );

    void setDocument( QString key );

    MarbleMap *const q;

    // The model we are showing.
    MarbleModel     *const m_model;
    bool             m_modelIsOwned;

    // Parameters for the maps appearance.
    ViewParams       m_viewParams;
    ViewportParams   m_viewport;
    bool             m_showFrameRate;


    LayerManager     m_layerManager;
    MarbleSplashLayer m_marbleSplashLayer;
    MarbleMap::CustomPaintLayer m_customPaintLayer;
    GeometryLayer            m_geometryLayer;
    FogLayer                 m_fogLayer;
    GroundLayer              m_groundLayer;
    TextureLayer     m_textureLayer;
    PlacemarkLayer   m_placemarkLayer;
    VectorTileLayer  m_vectorTileLayer;

    bool m_isLockedToSubSolarPoint;
    bool m_isSubSolarPointIconVisible;
    RenderState m_renderState;
};

MarbleMapPrivate::MarbleMapPrivate( MarbleMap *parent, MarbleModel *model ) :
    q( parent ),
    m_model( model ),
    m_viewParams(),
    m_showFrameRate( false ),
    m_layerManager( model, parent ),
    m_customPaintLayer( parent ),
    m_geometryLayer( model->treeModel() ),
    m_textureLayer( model->downloadManager(), model->sunLocator(), model->groundOverlayModel() ),
    m_placemarkLayer( model->placemarkModel(), model->placemarkSelectionModel(), model->clock() ),
    m_vectorTileLayer( model->downloadManager(), model->pluginManager(), model->treeModel() ),
    m_isLockedToSubSolarPoint( false ),
    m_isSubSolarPointIconVisible( false )
{
    m_layerManager.addLayer( &m_fogLayer );
    m_layerManager.addLayer( &m_groundLayer );
    m_layerManager.addLayer( &m_geometryLayer );
    m_layerManager.addLayer( &m_placemarkLayer );
    m_layerManager.addLayer( &m_customPaintLayer );

    QObject::connect( m_model, SIGNAL(themeChanged(QString)),
                      parent, SLOT(updateMapTheme()) );
    QObject::connect( m_model->fileManager(), SIGNAL(fileAdded(QString)),
                      parent, SLOT(setDocument(QString)) );


    QObject::connect( &m_placemarkLayer, SIGNAL(repaintNeeded()),
                      parent, SIGNAL(repaintNeeded()));

    QObject::connect ( &m_layerManager, SIGNAL(pluginSettingsChanged()),
                       parent,        SIGNAL(pluginSettingsChanged()) );
    QObject::connect ( &m_layerManager, SIGNAL(repaintNeeded(QRegion)),
                       parent,        SIGNAL(repaintNeeded(QRegion)) );
    QObject::connect ( &m_layerManager, SIGNAL(renderPluginInitialized(RenderPlugin*)),
                       parent,        SIGNAL(renderPluginInitialized(RenderPlugin*)) );
    QObject::connect ( &m_layerManager, SIGNAL(visibilityChanged(QString,bool)),
                       parent,        SLOT(setPropertyValue(QString,bool)) );

    QObject::connect( &m_geometryLayer, SIGNAL(repaintNeeded()),
                      parent, SIGNAL(repaintNeeded()));

    /**
     * Slot handleHighlight finds all placemarks
     * that contain the clicked point.
     * The placemarks under the clicked position may
     * have their styleUrl set to a style map which
     * doesn't specify any highlight styleId. Such
     * placemarks will be fletered out in GeoGraphicsScene
     * and will not be highlighted.
     */
    QObject::connect( parent, SIGNAL(highlightedPlacemarksChanged(qreal,qreal,GeoDataCoordinates::Unit)),
                      &m_geometryLayer, SLOT(handleHighlight(qreal,qreal,GeoDataCoordinates::Unit)) );

    QObject::connect( &m_textureLayer, SIGNAL(tileLevelChanged(int)),
                      parent, SIGNAL(tileLevelChanged(int)) );
    QObject::connect( &m_textureLayer, SIGNAL(repaintNeeded()),
                      parent, SIGNAL(repaintNeeded()) );

    QObject::connect( parent, SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)),
                      parent, SIGNAL(repaintNeeded()) );
}

void MarbleMapPrivate::updateProperty( const QString &name, bool show )
{
    // earth
    if ( name == "places" ) {
        m_placemarkLayer.setShowPlaces( show );
    } else if ( name == "cities" ) {
        m_placemarkLayer.setShowCities( show );
    } else if ( name == "terrain" ) {
        m_placemarkLayer.setShowTerrain( show );
    } else if ( name == "otherplaces" ) {
        m_placemarkLayer.setShowOtherPlaces( show );
    }

    // other planets
    else if ( name == "landingsites" ) {
        m_placemarkLayer.setShowLandingSites( show );
    } else if ( name == "craters" ) {
        m_placemarkLayer.setShowCraters( show );
    } else if ( name == "maria" ) {
        m_placemarkLayer.setShowMaria( show );
    }

    else if ( name == "relief" ) {
        m_textureLayer.setShowRelief( show );
    }

    foreach( RenderPlugin *renderPlugin, m_layerManager.renderPlugins() ) {
        if ( name == renderPlugin->nameId() ) {
            if ( renderPlugin->visible() == show ) {
                break;
            }

            renderPlugin->setVisible( show );

            break;
        }
    }
}

// ----------------------------------------------------------------


MarbleMap::MarbleMap()
    : d( new MarbleMapPrivate( this, new MarbleModel( this ) ) )
{
#ifdef MARBLE_DBUS
    QDBusConnection::sessionBus().registerObject( "/MarbleMap", this,
                                                  QDBusConnection::ExportAllSlots
                                                  | QDBusConnection::ExportAllSignals
                                                  | QDBusConnection::ExportAllProperties );
#endif
}

MarbleMap::MarbleMap(MarbleModel *model)
    : d( new MarbleMapPrivate( this, model ) )
{
#ifdef MARBLE_DBUS
    QDBusConnection::sessionBus().registerObject( "/MarbleMap", this,
                                                  QDBusConnection::ExportAllSlots
                                                  | QDBusConnection::ExportAllSignals
                                                  | QDBusConnection::ExportAllProperties );
#endif

    d->m_modelIsOwned = false;
}

MarbleMap::~MarbleMap()
{
    MarbleModel *model = d->m_modelIsOwned ? d->m_model : 0;

    d->m_layerManager.removeLayer( &d->m_customPaintLayer );
    d->m_layerManager.removeLayer( &d->m_geometryLayer );
    d->m_layerManager.removeLayer( &d->m_fogLayer );
    d->m_layerManager.removeLayer( &d->m_placemarkLayer );
    d->m_layerManager.removeLayer( &d->m_textureLayer );
    d->m_layerManager.removeLayer( &d->m_groundLayer );
    delete d;

    delete model;  // delete the model after private data
}

MarbleModel *MarbleMap::model() const
{
    return d->m_model;
}

ViewportParams *MarbleMap::viewport()
{
    return &d->m_viewport;
}

const ViewportParams *MarbleMap::viewport() const
{
    return &d->m_viewport;
}


void MarbleMap::setMapQualityForViewContext( MapQuality quality, ViewContext viewContext )
{
    d->m_viewParams.setMapQualityForViewContext( quality, viewContext );

    // Update texture map during the repaint that follows:
    d->m_textureLayer.setNeedsUpdate();
}

MapQuality MarbleMap::mapQuality( ViewContext viewContext ) const
{
    return d->m_viewParams.mapQuality( viewContext );
}

MapQuality MarbleMap::mapQuality() const
{
    return d->m_viewParams.mapQuality();
}

void MarbleMap::setViewContext( ViewContext viewContext )
{
    const MapQuality oldQuality = d->m_viewParams.mapQuality();

    d->m_viewParams.setViewContext( viewContext );

    if ( d->m_viewParams.mapQuality() != oldQuality ) {
        // Update texture map during the repaint that follows:
        d->m_textureLayer.setNeedsUpdate();

        emit repaintNeeded();
    }
}

ViewContext MarbleMap::viewContext() const
{
    return d->m_viewParams.viewContext();
}


void MarbleMap::setSize( int width, int height )
{
    setSize( QSize( width, height ) );
}

void MarbleMap::setSize( const QSize& size )
{
    d->m_viewport.setSize( size );

    emit visibleLatLonAltBoxChanged( d->m_viewport.viewLatLonAltBox() );
}

QSize MarbleMap::size() const
{
    return QSize( d->m_viewport.width(), d->m_viewport.height() );
}

int  MarbleMap::width() const
{
    return d->m_viewport.width();
}

int  MarbleMap::height() const
{
    return d->m_viewport.height();
}

int MarbleMap::radius() const
{
    return d->m_viewport.radius();
}

void MarbleMap::setRadius( int radius )
{
    const int oldRadius = d->m_viewport.radius();

    d->m_viewport.setRadius( radius );

    if ( oldRadius != d->m_viewport.radius() ) {
        emit radiusChanged( radius );
        emit visibleLatLonAltBoxChanged( d->m_viewport.viewLatLonAltBox() );
    }
}


int MarbleMap::preferredRadiusCeil( int radius )
{
    if ( !d->m_layerManager.internalLayers().contains( &d->m_textureLayer ) )
        return radius;

    return d->m_textureLayer.preferredRadiusCeil( radius );
}


int MarbleMap::preferredRadiusFloor( int radius )
{
    if ( !d->m_layerManager.internalLayers().contains( &d->m_textureLayer ) )
        return radius;

    return d->m_textureLayer.preferredRadiusFloor( radius );
}


int MarbleMap::tileZoomLevel() const
{
    return d->m_textureLayer.tileZoomLevel();
}


qreal MarbleMap::centerLatitude() const
{
    // Calculate translation of center point
    const qreal centerLat = d->m_viewport.centerLatitude();

    return centerLat * RAD2DEG;
}

qreal MarbleMap::centerLongitude() const
{
    // Calculate translation of center point
    const qreal centerLon = d->m_viewport.centerLongitude();

    return centerLon * RAD2DEG;
}

int  MarbleMap::minimumZoom() const
{
    if ( d->m_model->mapTheme() )
        return d->m_model->mapTheme()->head()->zoom()->minimum();

    return 950;
}

int  MarbleMap::maximumZoom() const
{
    if ( d->m_model->mapTheme() )
        return d->m_model->mapTheme()->head()->zoom()->maximum();

    return 2100;
}

QVector<const GeoDataFeature*> MarbleMap::whichFeatureAt( const QPoint& curpos ) const
{
    return d->m_placemarkLayer.whichPlacemarkAt( curpos ) + d->m_geometryLayer.whichFeatureAt( curpos, viewport() );
}

void MarbleMap::reload()
{
    d->m_textureLayer.reload();
}

void MarbleMap::downloadRegion( QVector<TileCoordsPyramid> const & pyramid )
{
    Q_ASSERT( textureLayer() );
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
            mDebug() << "MarbleMap::downloadRegion level:" << level << "tile coords:" << coords;
            int x1, y1, x2, y2;
            coords.getCoords( &x1, &y1, &x2, &y2 );
            for ( int x = x1; x <= x2; ++x ) {
                for ( int y = y1; y <= y2; ++y ) {
                    TileId const stackedTileId( 0, level, x, y );
                    tileIdSet.insert( stackedTileId );
                    // FIXME: use lazy evaluation to not generate up to 100k tiles in one go
                    // this can take considerable time even on very fast systems
                    // in contrast generating the TileIds on the fly when they are needed
                    // does not seem to affect download speed.
                }
            }
        }
        QSetIterator<TileId> i( tileIdSet );
        while( i.hasNext() ) {
            TileId const tileId = i.next();
            d->m_textureLayer.downloadStackedTile( tileId );
        }
        tilesCount += tileIdSet.count();
    }
    // Needed for downloading unique tiles only. Much faster than if tiles for each level is downloaded separately

    int const elapsedMs = t.elapsed();
    mDebug() << "MarbleMap::downloadRegion:" << tilesCount << "tiles, " << elapsedMs << "ms";
}

bool MarbleMap::propertyValue( const QString& name ) const
{
    bool value;
    if ( d->m_model->mapTheme() ) {
        d->m_model->mapTheme()->settings()->propertyValue( name, value );
    }
    else {
        value = false;
        mDebug() << "WARNING: Failed to access a map theme! Property: " << name;
    }
    return value;
}

bool MarbleMap::showOverviewMap() const
{
    return propertyValue( "overviewmap" );
}

bool MarbleMap::showScaleBar() const
{
    return propertyValue( "scalebar" );
}

bool MarbleMap::showCompass() const
{
    return propertyValue( "compass" );
}

bool MarbleMap::showGrid() const
{
    return propertyValue( "coordinate-grid" );
}

bool MarbleMap::showClouds() const
{
    return d->m_viewParams.showClouds();
}

bool MarbleMap::showSunShading() const
{
    return d->m_textureLayer.showSunShading();
}

bool MarbleMap::showCityLights() const
{
    return d->m_textureLayer.showCityLights();
}

bool MarbleMap::isLockedToSubSolarPoint() const
{
    return d->m_isLockedToSubSolarPoint;
}

bool MarbleMap::isSubSolarPointIconVisible() const
{
    return d->m_isSubSolarPointIconVisible;
}

bool MarbleMap::showAtmosphere() const
{
    return d->m_viewParams.showAtmosphere();
}

bool MarbleMap::showCrosshairs() const
{
    bool visible = false;

    QList<RenderPlugin *> pluginList = renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = pluginList.constEnd();
    for (; i != end; ++i ) {
        if ( (*i)->nameId() == "crosshairs" ) {
            visible = (*i)->visible();
        }
    }

    return visible;
}

bool MarbleMap::showPlaces() const
{
    return propertyValue( "places" );
}

bool MarbleMap::showCities() const
{
    return propertyValue( "cities" );
}

bool MarbleMap::showTerrain() const
{
    return propertyValue( "terrain" );
}

bool MarbleMap::showOtherPlaces() const
{
    return propertyValue( "otherplaces" );
}

bool MarbleMap::showRelief() const
{
    return propertyValue( "relief" );
}

bool MarbleMap::showIceLayer() const
{
    return propertyValue( "ice" );
}

bool MarbleMap::showBorders() const
{
    return propertyValue( "borders" );
}

bool MarbleMap::showRivers() const
{
    return propertyValue( "rivers" );
}

bool MarbleMap::showLakes() const
{
    return propertyValue( "lakes" );
}

bool MarbleMap::showFrameRate() const
{
    return d->m_showFrameRate;
}

bool MarbleMap::showBackground() const
{
    return d->m_layerManager.showBackground();
}

quint64 MarbleMap::volatileTileCacheLimit() const
{
    return d->m_textureLayer.volatileCacheLimit();
}


void MarbleMap::rotateBy( const qreal& deltaLon, const qreal& deltaLat )
{
    centerOn( d->m_viewport.centerLongitude() * RAD2DEG + deltaLon,
              d->m_viewport.centerLatitude()  * RAD2DEG + deltaLat );
}


void MarbleMap::centerOn( const qreal lon, const qreal lat )
{
    d->m_viewport.centerOn( lon * DEG2RAD, lat * DEG2RAD );

    emit visibleLatLonAltBoxChanged( d->m_viewport.viewLatLonAltBox() );
}

void MarbleMap::setCenterLatitude( qreal lat )
{
    centerOn( centerLongitude(), lat );
}

void MarbleMap::setCenterLongitude( qreal lon )
{
    centerOn( lon, centerLatitude() );
}

Projection MarbleMap::projection() const
{
    return d->m_viewport.projection();
}

void MarbleMap::setProjection( Projection projection )
{
    if ( d->m_viewport.projection() == projection )
        return;

    emit projectionChanged( projection );

    d->m_viewport.setProjection( projection );

    d->m_textureLayer.setProjection( projection );

    emit visibleLatLonAltBoxChanged( d->m_viewport.viewLatLonAltBox() );
}


bool MarbleMap::screenCoordinates( qreal lon, qreal lat,
                                   qreal& x, qreal& y ) const
{
    return d->m_viewport.screenCoordinates( lon * DEG2RAD, lat * DEG2RAD, x, y );
}

bool MarbleMap::geoCoordinates( int x, int y,
                                qreal& lon, qreal& lat,
                                GeoDataCoordinates::Unit unit ) const
{
    return d->m_viewport.geoCoordinates( x, y, lon, lat, unit );
}

void MarbleMapPrivate::setDocument( QString key )
{
    if ( !m_model->mapTheme() ) {
        // Happens if no valid map theme is set or at application startup
        // if a file is passed via command line parameters and the last
        // map theme has not been loaded yet
        /**
         * @todo Do we need to queue the document and process it once a map
         * theme becomes available?
         */
        return;
    }

    GeoDataDocument* doc = m_model->fileManager()->at( key );

    foreach ( const GeoSceneLayer *layer, m_model->mapTheme()->map()->layers() ) {
        if ( layer->backend() != dgml::dgmlValue_geodata
             && layer->backend() != dgml::dgmlValue_vector )
            continue;

        // look for documents
        foreach ( const GeoSceneAbstractDataset *dataset, layer->datasets() ) {
            const GeoSceneGeodata *data = static_cast<const GeoSceneGeodata*>( dataset );
            QString containername = data->sourceFile();
            QString colorize = data->colorize();
            if( key == containername ) {
                if( colorize == "land" ) {
                    m_textureLayer.addLandDocument( doc );
                }
                if( colorize == "sea" ) {
                    m_textureLayer.addSeaDocument( doc );
                }

                // set visibility according to theme property
                if( !data->property().isEmpty() ) {
                    bool value;
                    m_model->mapTheme()->settings()->propertyValue( data->property(), value );
                    doc->setVisible( value );
                    m_model->treeModel()->updateFeature( doc );
                }
            }
        }
    }
}

// Used to be paintEvent()
void MarbleMap::paint( GeoPainter &painter, const QRect &dirtyRect )
{
    Q_UNUSED( dirtyRect );

    if ( !d->m_model->mapTheme() ) {
        mDebug() << "No theme yet!";
        d->m_marbleSplashLayer.render( &painter, &d->m_viewport );
        return;
    }

    QTime t;
    t.start();

    RenderStatus const oldRenderStatus = d->m_renderState.status();
    d->m_layerManager.renderLayers( &painter, &d->m_viewport );
    d->m_renderState = d->m_layerManager.renderState();
    bool const parsing = d->m_model->fileManager()->pendingFiles() > 0;
    d->m_renderState.addChild( RenderState( "Files", parsing ? WaitingForData : Complete ) );
    RenderStatus const newRenderStatus = d->m_renderState.status();
    if ( oldRenderStatus != newRenderStatus ) {
        emit renderStatusChanged( newRenderStatus );
    }
    emit renderStateChanged( d->m_renderState );

    if ( d->m_showFrameRate ) {
        FpsLayer fpsPainter( &t );
        fpsPainter.paint( &painter );
    }

    const qreal fps = 1000.0 / (qreal)( t.elapsed() );
    emit framesPerSecond( fps );
}

void MarbleMap::customPaint( GeoPainter *painter )
{
    Q_UNUSED( painter );
}

QString MarbleMap::mapThemeId() const
{
    return d->m_model->mapThemeId();
}

void MarbleMap::setMapThemeId( const QString& mapThemeId )
{
    d->m_model->setMapThemeId( mapThemeId );
}

void MarbleMapPrivate::updateMapTheme()
{
    m_layerManager.removeLayer( &m_textureLayer );
    // FIXME Find a better way to do this reset. Maybe connect to themeChanged SIGNAL?
    m_vectorTileLayer.reset();
    m_layerManager.removeLayer( &m_vectorTileLayer );
    m_layerManager.removeLayer( &m_groundLayer );

    QObject::connect( m_model->mapTheme()->settings(), SIGNAL(valueChanged(QString,bool)),
                      q, SLOT(updateProperty(QString,bool)) );
    QObject::connect( m_model->mapTheme()->settings(), SIGNAL(valueChanged(QString,bool)),
                      m_model, SLOT(updateProperty(QString,bool)) );

    q->setPropertyValue( "clouds_data", m_viewParams.showClouds() );

    if ( !m_model->mapTheme()->map()->hasTextureLayers() ) {
        m_groundLayer.setColor( m_model->mapTheme()->map()->backgroundColor() );
        m_layerManager.addLayer( &m_groundLayer );
    }

    // Check whether there is a texture layer and vectortile layer available:
    if ( m_model->mapTheme()->map()->hasTextureLayers() ) {
        const GeoSceneSettings *const settings = m_model->mapTheme()->settings();
        const GeoSceneGroup *const textureLayerSettings = settings ? settings->group( "Texture Layers" ) : 0;
        const GeoSceneGroup *const vectorTileLayerSettings = settings ? settings->group( "VectorTile Layers" ) : 0;

        bool textureLayersOk = true;
        bool vectorTileLayersOk = true;

        // textures will contain texture layers and
        // vectorTiles vectortile layers
        QVector<const GeoSceneTextureTile *> textures;
        QVector<const GeoSceneVectorTile *> vectorTiles;

        foreach( GeoSceneLayer* layer, m_model->mapTheme()->map()->layers() ){
            if ( layer->backend() == dgml::dgmlValue_texture ){

                foreach ( const GeoSceneAbstractDataset *pos, layer->datasets() ) {
                    const GeoSceneTextureTile *const texture = dynamic_cast<GeoSceneTextureTile const *>( pos );
                    if ( !texture )
                        continue;

                    const QString sourceDir = texture->sourceDir();
                    const QString installMap = texture->installMap();
                    const QString role = layer->role();

                    // If the tiles aren't already there, put up a progress dialog
                    // while creating them.
                    if ( !TileLoader::baseTilesAvailable( *texture )
                         && !installMap.isEmpty() )
                    {
                        mDebug() << "Base tiles not available. Creating Tiles ... \n"
                                 << "SourceDir: " << sourceDir << "InstallMap:" << installMap;

                        TileCreator *tileCreator = new TileCreator(
                                    sourceDir,
                                    installMap,
                                    (role == "dem") ? "true" : "false" );
                        tileCreator->setTileFormat( texture->fileFormat().toLower() );

                        QPointer<TileCreatorDialog> tileCreatorDlg = new TileCreatorDialog( tileCreator, 0 );
                        tileCreatorDlg->setSummary( m_model->mapTheme()->head()->name(),
                                                    m_model->mapTheme()->head()->description() );
                        tileCreatorDlg->exec();
                        if ( TileLoader::baseTilesAvailable( *texture ) ) {
                            mDebug() << "Base tiles for" << sourceDir << "successfully created.";
                        } else {
                            qWarning() << "Some or all base tiles for" << sourceDir << "could not be created.";
                        }

                        delete tileCreatorDlg;
                    }

                    if ( TileLoader::baseTilesAvailable( *texture ) ) {
                        textures.append( texture );
                    } else {
                        qWarning() << "Base tiles for" << sourceDir << "not available. Skipping all texture layers.";
                        textureLayersOk = false;
                    }
                }
            }
            else if ( layer->backend() == dgml::dgmlValue_vectortile ){

                foreach ( const GeoSceneAbstractDataset *pos, layer->datasets() ) {
                    const GeoSceneVectorTile *const vectorTile = dynamic_cast<GeoSceneVectorTile const *>( pos );
                    if ( !vectorTile )
                        continue;

                    const QString sourceDir = vectorTile->sourceDir();
                    const QString installMap = vectorTile->installMap();
                    const QString role = layer->role();

                    // If the tiles aren't already there, put up a progress dialog
                    // while creating them.
                    if ( !TileLoader::baseTilesAvailable( *vectorTile )
                         && !installMap.isEmpty() )
                    {
                        mDebug() << "Base tiles not available. Creating Tiles ... \n"
                                 << "SourceDir: " << sourceDir << "InstallMap:" << installMap;

                        TileCreator *tileCreator = new TileCreator(
                                    sourceDir,
                                    installMap,
                                    (role == "dem") ? "true" : "false" );
                        tileCreator->setTileFormat( vectorTile->fileFormat().toLower() );

                        QPointer<TileCreatorDialog> tileCreatorDlg = new TileCreatorDialog( tileCreator, 0 );
                        tileCreatorDlg->setSummary( m_model->mapTheme()->head()->name(),
                                                    m_model->mapTheme()->head()->description() );
                        tileCreatorDlg->exec();
                        if ( TileLoader::baseTilesAvailable( *vectorTile ) ) {
                            qDebug() << "Base tiles for" << sourceDir << "successfully created.";
                        } else {
                            qDebug() << "Some or all base tiles for" << sourceDir << "could not be created.";
                        }

                        delete tileCreatorDlg;
                    }

                    if ( TileLoader::baseTilesAvailable( *vectorTile ) ) {
                        vectorTiles.append( vectorTile );
                    } else {
                        qWarning() << "Base tiles for" << sourceDir << "not available. Skipping all texture layers.";
                        vectorTileLayersOk = false;
                    }
                }
            }
        }

        QString seafile, landfile;
        if( !m_model->mapTheme()->map()->filters().isEmpty() ) {
            const GeoSceneFilter *filter= m_model->mapTheme()->map()->filters().first();

            if( filter->type() == "colorize" ) {
                //no need to look up with MarbleDirs twice so they are left null for now
                QList<const GeoScenePalette*> palette = filter->palette();
                foreach (const GeoScenePalette *curPalette, palette ) {

                    if( curPalette->type() == "sea" ) {
                        seafile = MarbleDirs::path( curPalette->file() );
                    } else if( curPalette->type() == "land" ) {
                        landfile = MarbleDirs::path( curPalette->file() );
                    }
                }
                //look up locations if they are empty
                if( seafile.isEmpty() )
                    seafile = MarbleDirs::path( "seacolors.leg" );
                if( landfile.isEmpty() )
                    landfile = MarbleDirs::path( "landcolors.leg" );
            }
        }

        m_textureLayer.setMapTheme( textures, textureLayerSettings, seafile, landfile );
        m_textureLayer.setProjection( m_viewport.projection() );
        m_textureLayer.setShowRelief( q->showRelief() );

        m_vectorTileLayer.setMapTheme( vectorTiles, vectorTileLayerSettings );

        if ( textureLayersOk )
            m_layerManager.addLayer( &m_textureLayer );
        if ( vectorTileLayersOk )
            m_layerManager.addLayer( &m_vectorTileLayer );
    }
    else {
        m_textureLayer.setMapTheme( QVector<const GeoSceneTextureTile *>(), 0, "", "" );
        m_vectorTileLayer.setMapTheme( QVector<const GeoSceneVectorTile *>(), 0 );
    }

    // earth
    m_placemarkLayer.setShowPlaces( q->showPlaces() );

    m_placemarkLayer.setShowCities( q->showCities() );
    m_placemarkLayer.setShowTerrain( q->showTerrain() );
    m_placemarkLayer.setShowOtherPlaces( q->showOtherPlaces() );
    m_placemarkLayer.setShowLandingSites( q->propertyValue("landingsites") );
    m_placemarkLayer.setShowCraters( q->propertyValue("craters") );
    m_placemarkLayer.setShowMaria( q->propertyValue("maria") );

    GeoDataFeature::setDefaultLabelColor( m_model->mapTheme()->map()->labelColor() );
    m_placemarkLayer.requestStyleReset();

    foreach( RenderPlugin *renderPlugin, m_layerManager.renderPlugins() ) {
        bool propertyAvailable = false;
        m_model->mapTheme()->settings()->propertyAvailable( renderPlugin->nameId(), propertyAvailable );
        bool propertyValue = false;
        m_model->mapTheme()->settings()->propertyValue( renderPlugin->nameId(), propertyValue );

        if ( propertyAvailable ) {
            renderPlugin->setVisible( propertyValue );
        }
    }

    emit q->themeChanged( m_model->mapTheme()->head()->mapThemeId() );
}

void MarbleMap::setPropertyValue( const QString& name, bool value )
{
    mDebug() << "In MarbleMap the property " << name << "was set to " << value;
    if ( d->m_model->mapTheme() ) {
        d->m_model->mapTheme()->settings()->setPropertyValue( name, value );
        d->m_textureLayer.setNeedsUpdate();
    }
    else {
        mDebug() << "WARNING: Failed to access a map theme! Property: " << name;
    }
}

void MarbleMap::setShowOverviewMap( bool visible )
{
    setPropertyValue( "overviewmap", visible );
}

void MarbleMap::setShowScaleBar( bool visible )
{
    setPropertyValue( "scalebar", visible );
}

void MarbleMap::setShowCompass( bool visible )
{
    setPropertyValue( "compass", visible );
}

void MarbleMap::setShowAtmosphere( bool visible )
{
    foreach ( RenderPlugin *plugin, renderPlugins() ) {
        if ( plugin->nameId() == "atmosphere" ) {
            plugin->setVisible( visible );
        }
    }

    d->m_viewParams.setShowAtmosphere( visible );
}

void MarbleMap::setShowCrosshairs( bool visible )
{
    QList<RenderPlugin *> pluginList = renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = pluginList.constEnd();
    for (; i != end; ++i ) {
        if ( (*i)->nameId() == "crosshairs" ) {
            (*i)->setVisible( visible );
        }
    }
}

void MarbleMap::setShowClouds( bool visible )
{
    d->m_viewParams.setShowClouds( visible );

    setPropertyValue( "clouds_data", visible );
}

void MarbleMap::setShowSunShading( bool visible )
{
    d->m_textureLayer.setShowSunShading( visible );
}

void MarbleMap::setShowCityLights( bool visible )
{
    d->m_textureLayer.setShowCityLights( visible );
    setPropertyValue( "citylights", visible );
}

void MarbleMap::setLockToSubSolarPoint( bool visible )
{
    disconnect( d->m_model->sunLocator(), SIGNAL(positionChanged(qreal,qreal)),
                this,                     SLOT(centerOn(qreal,qreal)) );

    if( isLockedToSubSolarPoint() != visible ) {
        d->m_isLockedToSubSolarPoint = visible;
    }

    if ( isLockedToSubSolarPoint() ) {
        connect( d->m_model->sunLocator(), SIGNAL(positionChanged(qreal,qreal)),
                 this,                     SLOT(centerOn(qreal,qreal)) );

        centerOn( d->m_model->sunLocator()->getLon(), d->m_model->sunLocator()->getLat() );
    } else if ( visible ) {
        mDebug() << "Ignoring centering on sun, since the sun plugin is not loaded.";
    }
}

void MarbleMap::setSubSolarPointIconVisible( bool visible )
{
    if ( isSubSolarPointIconVisible() != visible ) {
        d->m_isSubSolarPointIconVisible = visible;
    }
}

void MarbleMap::setShowTileId( bool visible )
{
    d->m_textureLayer.setShowTileId( visible );
}

void MarbleMap::setShowGrid( bool visible )
{
    setPropertyValue( "coordinate-grid", visible );
}

void MarbleMap::setShowPlaces( bool visible )
{
    setPropertyValue( "places", visible );
}

void MarbleMap::setShowCities( bool visible )
{
    setPropertyValue( "cities", visible );
}

void MarbleMap::setShowTerrain( bool visible )
{
    setPropertyValue( "terrain", visible );
}

void MarbleMap::setShowOtherPlaces( bool visible )
{
    setPropertyValue( "otherplaces", visible );
}

void MarbleMap::setShowRelief( bool visible )
{
    setPropertyValue( "relief", visible );
}

void MarbleMap::setShowIceLayer( bool visible )
{
    setPropertyValue( "ice", visible );
}

void MarbleMap::setShowBorders( bool visible )
{
    setPropertyValue( "borders", visible );
}

void MarbleMap::setShowRivers( bool visible )
{
    setPropertyValue( "rivers", visible );
}

void MarbleMap::setShowLakes( bool visible )
{
    setPropertyValue( "lakes", visible );
}

void MarbleMap::setShowFrameRate( bool visible )
{
    d->m_showFrameRate = visible;
}

void MarbleMap::setShowRuntimeTrace( bool visible )
{
    d->m_layerManager.setShowRuntimeTrace( visible );
}

void MarbleMap::setShowBackground( bool visible )
{
    d->m_layerManager.setShowBackground( visible );
}

void MarbleMap::notifyMouseClick( int x, int y )
{
    qreal  lon   = 0;
    qreal  lat   = 0;

    const bool valid = geoCoordinates( x, y, lon, lat, GeoDataCoordinates::Radian );

    if ( valid ) {
        emit mouseClickGeoPosition( lon, lat, GeoDataCoordinates::Radian );
    }
}

void MarbleMap::clearVolatileTileCache()
{
    d->m_vectorTileLayer.reset();
    d->m_textureLayer.reset();
    mDebug() << "Cleared Volatile Cache!";
}

void MarbleMap::setVolatileTileCacheLimit( quint64 kilobytes )
{
    mDebug() << "kiloBytes" << kilobytes;
    d->m_textureLayer.setVolatileCacheLimit( kilobytes );
}

AngleUnit MarbleMap::defaultAngleUnit() const
{
    if ( GeoDataCoordinates::defaultNotation() == GeoDataCoordinates::Decimal ) {
        return DecimalDegree;
    } else if ( GeoDataCoordinates::defaultNotation() == GeoDataCoordinates::UTM ) {
    	return UTM;
    }

    return DMSDegree;
}

void MarbleMap::setDefaultAngleUnit( AngleUnit angleUnit )
{
    if ( angleUnit == DecimalDegree ) {
        GeoDataCoordinates::setDefaultNotation( GeoDataCoordinates::Decimal );
        return;
    } else if ( angleUnit == UTM ) {
        GeoDataCoordinates::setDefaultNotation( GeoDataCoordinates::UTM );
        return;
    }

    GeoDataCoordinates::setDefaultNotation( GeoDataCoordinates::DMS );
}

QFont MarbleMap::defaultFont() const
{
    return GeoDataFeature::defaultFont();
}

void MarbleMap::setDefaultFont( const QFont& font )
{
    GeoDataFeature::setDefaultFont( font );
    d->m_placemarkLayer.requestStyleReset();
}

QList<RenderPlugin *> MarbleMap::renderPlugins() const
{
    return d->m_layerManager.renderPlugins();
}

QList<AbstractFloatItem *> MarbleMap::floatItems() const
{
    return d->m_layerManager.floatItems();
}

AbstractFloatItem * MarbleMap::floatItem( const QString &nameId ) const
{
    foreach ( AbstractFloatItem * floatItem, floatItems() ) {
        if ( floatItem && floatItem->nameId() == nameId ) {
            return floatItem;
        }
    }

    return 0; // No item found
}

QList<AbstractDataPlugin *> MarbleMap::dataPlugins()  const
{
    return d->m_layerManager.dataPlugins();
}

QList<AbstractDataPluginItem *> MarbleMap::whichItemAt( const QPoint& curpos ) const
{
    return d->m_layerManager.whichItemAt( curpos );
}

void MarbleMap::addLayer( LayerInterface *layer )
{
    d->m_layerManager.addLayer(layer);
}

void MarbleMap::removeLayer( LayerInterface *layer )
{
    d->m_layerManager.removeLayer(layer);
}

RenderStatus MarbleMap::renderStatus() const
{
    return d->m_layerManager.renderState().status();
}

RenderState MarbleMap::renderState() const
{
    return d->m_layerManager.renderState();
}

// this method will only temporarily "pollute" the MarbleModel class
TextureLayer *MarbleMap::textureLayer() const
{
    return &d->m_textureLayer;
}

}

#include "MarbleMap.moc"
