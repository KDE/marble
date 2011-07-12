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
// Copyright 2010-2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


// Own
#include "MarbleMap.h"

// Posix
#include <cmath>

// Qt
#include <QtCore/QAbstractItemModel>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtGui/QItemSelectionModel>
#include <QtGui/QSizePolicy>
#include <QtGui/QRegion>

#ifdef MARBLE_DBUS
#include <QtDBus/QDBusConnection>
#endif

// Marble
#include "AbstractFloatItem.h"
#include "AbstractProjection.h"
#include "AtmosphereLayer.h"
#include "FogLayer.h"
#include "FpsLayer.h"
#include "GeoDataDocument.h"
#include "GeoDataFeature.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataTreeModel.h"
#include "GeometryLayer.h"
#include "GeoPainter.h"
#include "GeoSceneDocument.h"
#include "GeoSceneFilter.h"
#include "GeoSceneHead.h"
#include "GeoSceneMap.h"
#include "GeoScenePalette.h"
#include "GeoSceneSettings.h"
#include "GeoSceneVector.h"
#include "GeoSceneZoom.h"
#include "LayerManager.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "MeasureTool.h"
#include "MergedLayerDecorator.h"
#include "PlacemarkLayout.h"
#include "Planet.h"
#include "RenderPlugin.h"
#include "SunLocator.h"
#include "TextureColorizer.h"
#include "TextureLayer.h"
#include "TileCoordsPyramid.h"
#include "TileCreator.h"
#include "TileCreatorDialog.h"
#include "TileLoader.h"
#include "VectorComposer.h"
#include "VectorMapBaseLayer.h"
#include "VectorMapLayer.h"
#include "ViewParams.h"
#include "ViewportParams.h"

namespace Marble
{

class MarbleMapPrivate
{
    friend class MarbleWidget;

 public:
    explicit MarbleMapPrivate( MarbleMap *parent, MarbleModel *model );

    void construct();

    void paintMarbleSplash( GeoPainter &painter, QRect &dirtyRect );

    void setBoundingBox();

    void updateProperty( const QString &, bool );

    MarbleMap       *m_parent;

    // The model we are showing.
    MarbleModel     *const m_model;
    bool             m_modelIsOwned;

    ViewParams       m_viewParams;
    bool             m_backgroundVisible;

    TextureColorizer *m_texcolorizer;

    LayerManager     m_layerManager;
    GeometryLayer           *m_geometryLayer;
    AtmosphereLayer          m_atmosphereLayer;
    FogLayer                 m_fogLayer;
    VectorMapBaseLayer       m_vectorMapBaseLayer;
    VectorMapLayer   m_vectorMapLayer;
    TextureLayer     m_textureLayer;
    PlacemarkLayout  m_placemarkLayout;
    VectorComposer   m_veccomposer;
    MeasureTool      m_measureTool;

    // Parameters for the maps appearance.

    bool             m_showFrameRate;
};

MarbleMapPrivate::MarbleMapPrivate( MarbleMap *parent, MarbleModel *model )
        : m_parent( parent ),
          m_model( model ),
          m_backgroundVisible( true ),
          m_texcolorizer( 0 ),
          m_layerManager( model, parent ),
          m_vectorMapBaseLayer( &m_veccomposer ),
          m_vectorMapLayer( &m_veccomposer ),
          m_textureLayer( model->mapThemeManager(), model->downloadManager(), model->sunLocator() ),
          m_placemarkLayout( model->placemarkModel(), model->placemarkSelectionModel(), parent ),
          m_measureTool( model )
{
    GeoDataDocument *document =  model->treeModel()->rootDocument();
    m_geometryLayer = new GeometryLayer( document );
    m_layerManager.addLayer( m_geometryLayer );

    m_layerManager.addLayer( &m_placemarkLayout );
    m_layerManager.addLayer( &m_fogLayer );
    m_layerManager.addLayer( &m_measureTool );
}

void MarbleMapPrivate::construct()
{
    m_parent->connect( m_model, SIGNAL( themeChanged( QString ) ),
                       m_parent, SIGNAL( themeChanged( QString ) ) );
    m_parent->connect( m_model, SIGNAL( modelChanged() ),
                       m_parent, SIGNAL( repaintNeeded() ) );

    m_parent->connect( &m_veccomposer, SIGNAL( datasetLoaded() ),
                       m_parent, SIGNAL( repaintNeeded() ));

    QObject::connect( m_model, SIGNAL( modelChanged() ),
                      &m_placemarkLayout, SLOT( setCacheData() ) );

    // FIXME: more on the spot update names and API
    QObject::connect ( &m_layerManager, SIGNAL( floatItemsChanged() ),
                       m_parent,        SIGNAL( repaintNeeded() ) );

    QObject::connect ( &m_layerManager, SIGNAL( pluginSettingsChanged() ),
                       m_parent,        SIGNAL( pluginSettingsChanged() ) );
    QObject::connect ( &m_layerManager, SIGNAL( repaintNeeded( QRegion ) ),
                       m_parent,        SIGNAL( repaintNeeded( QRegion ) ) );
    QObject::connect ( &m_layerManager, SIGNAL( renderPluginInitialized( RenderPlugin * ) ),
                       m_parent,        SIGNAL( renderPluginInitialized( RenderPlugin * ) ) );

    // FloatItems
    m_showFrameRate = false;


    m_parent->connect( m_model->sunLocator(), SIGNAL( updateSun() ),
                       &m_textureLayer,       SLOT( update() ) );
    m_parent->connect( m_model->sunLocator(), SIGNAL( centerSun( qreal, qreal ) ),
                       m_parent,              SLOT( centerOn( qreal, qreal ) ) );

    m_parent->connect( &m_textureLayer, SIGNAL( tileLevelChanged( int ) ),
                       m_parent, SIGNAL( tileLevelChanged( int ) ) );
    m_parent->connect( &m_textureLayer, SIGNAL( repaintNeeded( QRegion ) ),
                       m_parent, SIGNAL( repaintNeeded( QRegion ) ) );
}

void  MarbleMapPrivate::paintMarbleSplash( GeoPainter &painter, QRect &dirtyRect )
{
    Q_UNUSED( dirtyRect )

    painter.save();

    QPixmap logoPixmap( MarbleDirs::path( "svg/marble-logo-inverted-72dpi.png" ) );

    if ( logoPixmap.width() > m_parent->width() * 0.7
         || logoPixmap.height() > m_parent->height() * 0.7 )
    {
        logoPixmap = logoPixmap.scaled( QSize( m_parent->width(), m_parent->height() ) * 0.7,
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation );
    }

    QPoint logoPosition( ( m_parent->width()  - logoPixmap.width() ) / 2,
                            ( m_parent->height() - logoPixmap.height() ) / 2 );
    painter.drawPixmap( logoPosition, logoPixmap );

    QString message; // "Please assign a map theme!";

    painter.setPen( Qt::white );

    int yTop = logoPosition.y() + logoPixmap.height() + 10;
    QRect textRect( 0, yTop,
                    m_parent->width(), m_parent->height() - yTop );
    painter.drawText( textRect, Qt::AlignHCenter | Qt::AlignTop, message ); 

    painter.restore();
}

void MarbleMapPrivate::updateProperty( const QString &name, bool show )
{
    // earth
    if ( name == "places" ) {
        m_placemarkLayout.setShowPlaces( show );
    } else if ( name == "cities" ) {
        m_placemarkLayout.setShowCities( show );
    } else if ( name == "terrain" ) {
        m_placemarkLayout.setShowTerrain( show );
    } else if ( name == "otherplaces" ) {
        m_placemarkLayout.setShowOtherPlaces( show );
    }

    // other planets
    else if ( name == "landingsites" ) {
        m_placemarkLayout.setShowLandingSites( show );
    } else if ( name == "craters" ) {
        m_placemarkLayout.setShowCraters( show );
    } else if ( name == "maria" ) {
        m_placemarkLayout.setShowMaria( show );
    }

    else if ( name == "waterbodies" ) {
        m_veccomposer.setShowWaterBodies( show );
    } else if ( name == "lakes" ) {
        m_veccomposer.setShowLakes( show );
    } else if ( name == "ice" ) {
        m_veccomposer.setShowIce( show );
    } else if ( name == "coastlines" ) {
        m_veccomposer.setShowCoastLines( show );
    } else if ( name == "rivers" ) {
        m_veccomposer.setShowRivers( show );
    } else if ( name == "borders" ) {
        m_veccomposer.setShowBorders( show );
    }

    else if ( name == "relief" ) {
        if ( m_texcolorizer ) {
            m_texcolorizer->setShowRelief( show );
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
    QTime t;
    t.start();
    
    d->m_modelIsOwned = true;

    d->construct();
    qDebug("Model: Time elapsed: %d ms", t.elapsed());
}

MarbleMap::MarbleMap(MarbleModel *model)
    : d( new MarbleMapPrivate( this, model ) )
{
//     QDBusConnection::sessionBus().registerObject( "/marble", this,
//                                                   QDBusConnection::ExportAllSlots );

    d->m_modelIsOwned = false;

    d->construct();
}

MarbleMap::~MarbleMap()
{
    MarbleModel *model = d->m_modelIsOwned ? d->m_model : 0;

    d->m_layerManager.removeLayer( &d->m_measureTool );
    d->m_layerManager.removeLayer( &d->m_fogLayer );
    d->m_layerManager.removeLayer( &d->m_placemarkLayout );
    d->m_layerManager.removeLayer( &d->m_vectorMapLayer );
    d->m_layerManager.removeLayer( &d->m_vectorMapBaseLayer );
    delete d;

    delete model;  // delete the model after private data
}

MarbleModel *MarbleMap::model() const
{
    return d->m_model;
}

ViewportParams *MarbleMap::viewport()
{
    return d->m_viewParams.viewport();
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
    d->m_viewParams.setViewContext( viewContext );

    // Update texture map during the repaint that follows:
    d->m_textureLayer.setNeedsUpdate();
}

ViewContext MarbleMap::viewContext() const
{
    return d->m_viewParams.viewContext();
}


void MarbleMap::setSize( int width, int height )
{
    d->m_viewParams.setSize( width, height );
    emit visibleLatLonAltBoxChanged( d->m_viewParams.viewport()->viewLatLonAltBox() );

    d->m_textureLayer.setNeedsUpdate();
}

void MarbleMap::setSize( const QSize& size )
{
    setSize( size.width(), size.height() );
}

QSize MarbleMap::size() const
{
    return QSize( d->m_viewParams.width(), d->m_viewParams.height() );
}

int  MarbleMap::width() const
{
    return d->m_viewParams.width();
}

int  MarbleMap::height() const
{
    return d->m_viewParams.height();
}

Quaternion MarbleMap::planetAxis() const
{
    return d->m_viewParams.planetAxis();
}


int MarbleMap::radius() const
{
    return d->m_viewParams.radius();
}

void MarbleMap::setRadius( int radius )
{
    d->m_viewParams.setRadius( radius );

    d->m_textureLayer.setNeedsUpdate();
}


int MarbleMap::preferredRadiusCeil( int radius )
{
    if ( d->m_textureLayer.tileZoomLevel() < 0 )
        return radius;

    return d->m_textureLayer.preferredRadiusCeil( radius );
}


int MarbleMap::preferredRadiusFloor( int radius )
{
    if ( d->m_textureLayer.tileZoomLevel() < 0 )
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
    qreal  centerLon;
    qreal  centerLat;

    d->m_viewParams.centerCoordinates( centerLon, centerLat );
    return centerLat * RAD2DEG;
}

qreal MarbleMap::centerLongitude() const
{
    // Calculate translation of center point
    qreal  centerLon;
    qreal  centerLat;

    d->m_viewParams.centerCoordinates( centerLon, centerLat );
    return centerLon * RAD2DEG;
}

int  MarbleMap::minimumZoom() const
{
    if ( d->m_viewParams.mapTheme() )
        return d->m_viewParams.mapTheme()->head()->zoom()->minimum();

    return 950;
}

int  MarbleMap::maximumZoom() const
{
    if ( d->m_viewParams.mapTheme() )
        return d->m_viewParams.mapTheme()->head()->zoom()->maximum();

    return 2100;
}

QVector<const GeoDataPlacemark*> MarbleMap::whichFeatureAt( const QPoint& curpos ) const
{
    return d->m_placemarkLayout.whichPlacemarkAt( curpos );
}

QPixmap MarbleMap::mapScreenShot()
{
    QPixmap screenshotPixmap( size() );
    screenshotPixmap.fill( Qt::transparent );

    GeoPainter painter( &screenshotPixmap, viewport(),
                        PrintQuality );
    painter.begin( &screenshotPixmap );
    QRect dirtyRect( QPoint(), size() );
    paint( painter, dirtyRect );
    painter.end();

    return screenshotPixmap;
}

void MarbleMap::reload() const
{
    d->m_textureLayer.reload();
}

void MarbleMap::downloadRegion( const QString& sourceDir, QVector<TileCoordsPyramid> const & pyramid )
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
                    TileId const tileId( sourceDir, level, x, y );
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
              textureLayer()->downloadTile( i.next() );
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
    d->m_viewParams.propertyValue( name, value );
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

bool MarbleMap::showElevationModel() const
{
    return d->m_viewParams.showElevationModel();
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

bool MarbleMap::showGps() const
{
    return d->m_viewParams.showGps();
}

bool MarbleMap::showFrameRate() const
{
    return d->m_showFrameRate;
}

bool MarbleMap::showBackground() const
{
    return d->m_backgroundVisible;
}

quint64 MarbleMap::volatileTileCacheLimit() const
{
    return d->m_textureLayer.volatileCacheLimit();
}


void MarbleMap::rotateBy( const Quaternion& incRot )
{
    d->m_viewParams.setPlanetAxis( incRot * d->m_viewParams.planetAxis() );
    d->m_textureLayer.setNeedsUpdate();

    emit visibleLatLonAltBoxChanged( d->m_viewParams.viewport()->viewLatLonAltBox() );
}

void MarbleMap::rotateBy( const qreal& deltaLon, const qreal& deltaLat )
{
    Quaternion  rotPhi( 1.0, deltaLat / 180.0, 0.0, 0.0 );
    Quaternion  rotTheta( 1.0, 0.0, deltaLon / 180.0, 0.0 );

    Quaternion  axis = d->m_viewParams.planetAxis();
    axis = rotTheta * axis;
    axis *= rotPhi;
    axis.normalize();
    d->m_viewParams.setPlanetAxis( axis );
    d->m_textureLayer.setNeedsUpdate();

    emit visibleLatLonAltBoxChanged( d->m_viewParams.viewport()->viewLatLonAltBox() );
}


void MarbleMap::centerOn( const qreal lon, const qreal lat )
{
    Quaternion  quat;
    quat.createFromEuler( -lat * DEG2RAD, lon * DEG2RAD, 0.0 );
    d->m_viewParams.setPlanetAxis( quat );
    d->m_textureLayer.setNeedsUpdate();

    emit visibleLatLonAltBoxChanged( d->m_viewParams.viewport()->viewLatLonAltBox() );
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
    return d->m_viewParams.projection();
}

void MarbleMap::setProjection( Projection projection )
{
    emit projectionChanged( projection );

    d->m_viewParams.setProjection( projection );

    d->m_textureLayer.setupTextureMapper( projection );

    emit visibleLatLonAltBoxChanged( d->m_viewParams.viewport()->viewLatLonAltBox() );
}


bool MarbleMap::screenCoordinates( qreal lon, qreal lat,
                                   qreal& x, qreal& y ) const
{
    return d->m_viewParams.currentProjection()
        ->screenCoordinates( lon * DEG2RAD, lat * DEG2RAD,
                             d->m_viewParams.viewport(),
                             x, y );
}

bool MarbleMap::geoCoordinates( int x, int y,
                                qreal& lon, qreal& lat,
                                GeoDataCoordinates::Unit unit ) const
{
    return d->m_viewParams.currentProjection()
        ->geoCoordinates( x, y, d->m_viewParams.viewport(),
                          lon, lat, unit );
}

// Used to be paintEvent()
void MarbleMap::paint( GeoPainter &painter, QRect &dirtyRect )
{
    QTime t;
    t.start();
    
    if ( !d->m_viewParams.mapTheme() ) {
        mDebug() << "No theme yet!";
        d->paintMarbleSplash( painter, dirtyRect );
        return;
    }

    QStringList renderPositions;

    if ( d->m_backgroundVisible ) {
        renderPositions << "STARS" << "BEHIND_TARGET";
        d->m_layerManager.renderLayers( &painter, &d->m_viewParams, renderPositions );
    }

    if ( d->m_viewParams.showAtmosphere() ) {
        d->m_atmosphereLayer.render( &painter, d->m_viewParams.viewport() );
    }

    if ( d->m_model->mapTheme()->map()->hasTextureLayers() ) {
        d->m_textureLayer.paintGlobe( &painter, &d->m_viewParams, dirtyRect );
    }

    renderPositions.clear();
    renderPositions << "SURFACE" << "HOVERS_ABOVE_SURFACE" << "ATMOSPHERE"
                    << "ORBIT" << "ALWAYS_ON_TOP" << "FLOAT_ITEM" << "USER_TOOLS";
    d->m_layerManager.renderLayers( &painter, &d->m_viewParams, renderPositions );

    customPaint( &painter );

    if ( d->m_showFrameRate ) {
        FpsLayer fpsLayer( &t );
        fpsLayer.render( &painter, d->m_viewParams.viewport() );
    }

    const qreal fps = 1000.0 / (qreal)( t.elapsed() );
    emit framesPerSecond( fps );
}

void MarbleMap::customPaint( GeoPainter *painter )
{
    Q_UNUSED( painter );

    if ( !d->m_viewParams.mapTheme() ) {
        return;
    }
}

QString MarbleMap::mapThemeId() const
{
    return d->m_model->mapThemeId();
}

void MarbleMap::setMapThemeId( const QString& mapThemeId )
{
    if ( !mapThemeId.isEmpty() && mapThemeId == d->m_model->mapThemeId() )
        return;

    if ( d->m_model->mapTheme() ) {
        disconnect( d->m_model->mapTheme()->settings(), SIGNAL( valueChanged( const QString &, bool ) ),
                    this, SLOT( updateProperty( const QString &, bool ) ) );
    }

    d->m_viewParams.setMapThemeId( mapThemeId );
    GeoSceneDocument *mapTheme = d->m_viewParams.mapTheme();

    d->m_layerManager.removeLayer( &d->m_vectorMapLayer );
    d->m_layerManager.removeLayer( &d->m_vectorMapBaseLayer );

    d->m_textureLayer.setTextureColorizer( 0 );
    delete d->m_texcolorizer;
    d->m_texcolorizer = 0;

    if ( !mapTheme ) {
        return;
    }

    connect( mapTheme->settings(), SIGNAL( valueChanged( const QString &, bool ) ),
             this, SLOT( updateProperty( const QString &, bool ) ) );

    // NOTE due to frequent regressions: 
    // Do NOT take it for granted that there is any TEXTURE or VECTOR data AVAILABLE
    // at this point. Some themes do NOT have either vector or texture data!
    
    // Check whether there is a vector layer available:
    if ( mapTheme->map()->hasVectorLayers() ) {
        d->m_veccomposer.setShowWaterBodies( propertyValue( "waterbodies" ) );
        d->m_veccomposer.setShowLakes( propertyValue( "lakes" ) );
        d->m_veccomposer.setShowIce( propertyValue( "ice" ) );
        d->m_veccomposer.setShowCoastLines( propertyValue( "coastlines" ) );
        d->m_veccomposer.setShowRivers( propertyValue( "rivers" ) );
        d->m_veccomposer.setShowBorders( propertyValue( "borders" ) );

	// Set all the colors for the vector layers
        d->m_veccomposer.setOceanColor( mapTheme->map()->backgroundColor() );

        // Just as with textures, this is a workaround for DGML2 to
        // emulate the old behaviour.

        GeoSceneLayer *layer = mapTheme->map()->layer( "mwdbii" );
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

    if ( mapTheme->map()->hasVectorLayers() ) {
        if ( !mapTheme->map()->hasTextureLayers() ) {
            d->m_layerManager.addLayer( &d->m_vectorMapBaseLayer );
        }

        d->m_layerManager.addLayer( &d->m_vectorMapLayer );
    }

    // NOTE due to frequent regressions: 
    // Do NOT take it for granted that there is any TEXTURE or VECTOR data AVAILABLE
    // at this point.

    // Check whether there is a texture layer available:
    if ( mapTheme->map()->hasTextureLayers() ) {
	d->m_textureLayer.setMapTheme( mapTheme );
        // If the tiles aren't already there, put up a progress dialog
        // while creating them.

        // As long as we don't have an Layer Management Class we just lookup
        // the name of the layer that has the same name as the theme ID
        QString themeID = mapTheme->head()->theme();

        GeoSceneLayer *layer =
            static_cast<GeoSceneLayer*>( mapTheme->map()->layer( themeID ) );
        GeoSceneTexture *texture =
            static_cast<GeoSceneTexture*>( layer->groundDataset() );

        QString sourceDir = texture->sourceDir();
        QString installMap = texture->installMap();
        QString role = mapTheme->map()->layer( themeID )->role();

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
            tileCreatorDlg->setSummary( mapTheme->head()->name(),
                                        mapTheme->head()->description() );
            tileCreatorDlg->exec();
            qDebug("Tile creation completed");
            delete tileCreatorDlg;
        }

        d->m_textureLayer.setupTextureMapper( d->m_viewParams.projection() );

        if( !mapTheme->map()->filters().isEmpty() ) {
            GeoSceneFilter *filter= mapTheme->map()->filters().first();

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
                if( seafile.isEmpty() )
                    seafile = MarbleDirs::path( "seacolors.leg" );
                if( landfile.isEmpty() )
                    landfile = MarbleDirs::path( "landcolors.leg" );

                d->m_texcolorizer = new TextureColorizer( seafile, landfile, &d->m_veccomposer, this );
                d->m_texcolorizer->setShowRelief( showRelief() );

                d->m_textureLayer.setTextureColorizer( d->m_texcolorizer );
            }
        }
    }

    // NOTE due to frequent regressions: 
    // Do NOT take it for granted that there is any TEXTURE or VECTOR data AVAILABLE
    // at this point!

    // earth
    d->m_placemarkLayout.setShowPlaces( showPlaces() );
    d->m_placemarkLayout.setShowCities( showCities() );
    d->m_placemarkLayout.setShowTerrain( showTerrain() );
    d->m_placemarkLayout.setShowOtherPlaces( showOtherPlaces() );

    // other planets
    d->m_placemarkLayout.setShowLandingSites( propertyValue( "landingsites" ) );
    d->m_placemarkLayout.setShowCraters( propertyValue( "craters") );
    d->m_placemarkLayout.setShowMaria( propertyValue( "maria" ) );

    d->m_placemarkLayout.setDefaultLabelColor( mapTheme->map()->labelColor() );
    d->m_placemarkLayout.requestStyleReset();

    d->m_model->setMapTheme( mapTheme );

    SunLocator  *sunLocator = d->m_model->sunLocator();

    if ( sunLocator && sunLocator->getCentered() ) {
        qreal  lon = sunLocator->getLon();
        qreal  lat = sunLocator->getLat();
        centerOn( lon, lat );

        mDebug() << "Centering on Sun at " << lat << lon;
    }

    d->m_layerManager.syncViewParamsAndPlugins( mapTheme );
}

void MarbleMap::setPropertyValue( const QString& name, bool value )
{
    mDebug() << "In MarbleMap the property " << name << "was set to " << value;
    d->m_viewParams.setPropertyValue( name, value );
    d->m_textureLayer.setNeedsUpdate();
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
    d->m_textureLayer.setNeedsUpdate();
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
    // Update texture map during the repaint that follows:
    d->m_textureLayer.setNeedsUpdate();
}

void MarbleMap::setShowElevationModel( bool visible )
{
    d->m_viewParams.setShowElevationModel( visible );
    // Update texture map during the repaint that follows:
    d->m_textureLayer.setNeedsUpdate();
}

void MarbleMap::setShowIceLayer( bool visible )
{
    setPropertyValue( "ice", visible );
    // Update texture map during the repaint that follows:
    d->m_textureLayer.setNeedsUpdate();
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
    // Update texture map during the repaint that follows:
    d->m_textureLayer.setNeedsUpdate();
}

void MarbleMap::setShowFrameRate( bool visible )
{
    d->m_showFrameRate = visible;
}

void MarbleMap::setShowBackground( bool visible )
{
    d->m_backgroundVisible = visible;
}

void MarbleMap::setShowGps( bool visible )
{
    d->m_viewParams.setShowGps( visible );
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
    d->m_textureLayer.update();
    mDebug() << "Cleared Volatile Cache!";
}

void MarbleMap::setVolatileTileCacheLimit( quint64 kilobytes )
{
    mDebug() << "kiloBytes" << kilobytes;
    d->m_textureLayer.setVolatileCacheLimit( kilobytes );
}


bool MarbleMap::mapCoversViewport()
{
    return d->m_viewParams.viewport()->mapCoversViewport();
}

AngleUnit MarbleMap::defaultAngleUnit() const
{
    if ( GeoDataCoordinates::defaultNotation() == GeoDataCoordinates::Decimal ) {
        return DecimalDegree;
    }

    return DMSDegree;
}

void MarbleMap::setDefaultAngleUnit( AngleUnit angleUnit )
{
    if ( angleUnit == DecimalDegree ) {
        GeoDataCoordinates::setDefaultNotation( GeoDataCoordinates::Decimal );
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
    d->m_placemarkLayout.requestStyleReset();
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

MeasureTool *MarbleMap::measureTool()
{
    return &d->m_measureTool;
}

// this method will only temporarily "pollute" the MarbleModel class
TextureLayer* MarbleMap::textureLayer()
{
    return &d->m_textureLayer;
}

}

#include "MarbleMap.moc"
