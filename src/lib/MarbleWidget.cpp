//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#include "MarbleWidget.h"

#include <cmath>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QItemSelectionModel>
#include <QtGui/QSizePolicy>
#include <QtGui/QPaintEvent>
#include <QtGui/QRegion>
#include <QtGui/QStyleOptionGraphicsItem>
#include <QtNetwork/QNetworkProxy>

#ifdef MARBLE_DBUS
#include <QtDBus/QDBusConnection>
#endif

#include "AbstractProjection.h"
#include "MarbleDirs.h"
#include "MarbleMap.h"
#include "MarbleModel.h"
#include "Quaternion.h"
#include "ViewParams.h"
#include "GeoPainter.h"
#include "FileViewModel.h"
#include "GeoDataCoordinates.h"
#include "GpxFileViewItem.h"
#include "MarblePhysics.h"
#include "MarblePlacemarkModel.h"
#include "MarbleWidgetInputHandler.h"
#include "TileCreatorDialog.h"
#include "gps/GpsLayer.h"
#include "SunLocator.h"
#include "MergedLayerDecorator.h"
#include "AbstractProjectionHelper.h"
#include "ViewportParams.h"

#include "MarbleMap_p.h"

namespace Marble
{

#ifdef Q_CC_MSVC
# ifndef KDEWIN_MATH_H
   static long double sqrt(int a) { return sqrt((long double)a); }
# endif
#endif


class MarbleWidgetPrivate
{
 public:
    MarbleWidgetPrivate( MarbleMap *map, MarbleWidget *parent )
        : m_widget( parent ),
          m_map( map ),
          m_model( map->model() ),
          m_viewContext( Marble::Still ),
          m_stillQuality( Marble::High ), m_animationQuality( Marble::Low ),
          m_animationsEnabled( false ),
          m_inputhandler( 0 ),
          m_physics( new MarblePhysics( parent ) ),
          m_proxyHost(),
          m_proxyPort( 0 ),
          m_user(),
          m_password()
    {
    }

    ~MarbleWidgetPrivate()
    {
        delete m_map;
    }

    void  construct();

    MarbleWidget    *m_widget;
    // The model we are showing.
    MarbleMap       *m_map;
    MarbleModel     *m_model;   // Owned by m_map.  Don't delete.

    ViewContext     m_viewContext;

    MapQuality      m_stillQuality;
    MapQuality      m_animationQuality;

    bool m_animationsEnabled;

    // Some values from m_map, as they were last time we repainted.
    // To store them here will save some repaintings.
    int              m_logZoom;

    MarbleWidgetInputHandler  *m_inputhandler;

    MarblePhysics    *m_physics;

    QString          m_proxyHost;
    qint16           m_proxyPort;
    QString          m_user;
    QString          m_password;
};



MarbleWidget::MarbleWidget(QWidget *parent)
    : QWidget( parent ),
      d( new MarbleWidgetPrivate( new MarbleMap(), this ) )
{
    d->construct();
}


MarbleWidget::MarbleWidget(MarbleMap *map, QWidget *parent)
    : QWidget( parent ),
      d( new MarbleWidgetPrivate( map, this ) )
{
    d->construct();
}

MarbleWidget::~MarbleWidget()
{
    // Remove and delete an existing InputHandler
    // initiazized in d->construct()
    setInputHandler( 0 );

    delete d;
}

void MarbleWidgetPrivate::construct()
{
#ifdef MARBLE_DBUS
    QDBusConnection::sessionBus().registerObject("/MarbleWidget", m_widget, 
                    QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals | QDBusConnection::ExportAllProperties);
#endif

    // Widget settings
    m_widget->setMinimumSize( 200, 300 );
    m_widget->setFocusPolicy( Qt::WheelFocus );
    m_widget->setFocus( Qt::OtherFocusReason );

    // Initialize the map and forward some signals.
    m_map->setSize( m_widget->width(), m_widget->height() );
    m_map->viewParams()->setMapQuality( m_stillQuality );

    m_widget->connect( m_map,    SIGNAL( projectionChanged( Projection ) ),
                       m_widget, SIGNAL( projectionChanged( Projection ) ) );

    // When some fundamental things change in the model, we got to
    // show this in the view, i.e. here.
    m_widget->connect( m_model,  SIGNAL( themeChanged( QString ) ),
		       m_widget, SIGNAL( themeChanged( QString ) ) );
    m_widget->connect( m_model,  SIGNAL( modelChanged() ),
		       m_widget, SLOT( updateChangedMap() ) );

    // When some fundamental things change in the map, we got to show
    // this in the view, i.e. here.
    m_widget->connect( m_map,    SIGNAL( zoomChanged( int ) ),
                       m_widget, SIGNAL( zoomChanged( int ) ) );

    // Set background: black.
    m_widget->setPalette( QPalette ( Qt::black ) );

    // Set whether the black space gets displayed or the earth gets simply 
    // displayed on the widget background.
    m_widget->setAutoFillBackground( true );

    // Track the GPS current point at timely intervals.
    m_widget->connect( m_model, SIGNAL( timeout() ),
                       m_widget, SLOT( updateGps() ) );

    // Show a progress dialog when the model calculates new map tiles.
    m_widget->connect( m_model, SIGNAL( creatingTilesStart( TileCreator*, const QString&,
                                                            const QString& ) ),
                       m_widget, SLOT( creatingTilesStart( TileCreator*, const QString&,
                                                           const QString& ) ) );

    m_logZoom  = 0;

    m_widget->connect( m_model->sunLocator(), SIGNAL( reenableWidgetInput() ),
                       m_widget, SLOT( enableInput() ) );

    m_widget->connect( m_model->sunLocator(), SIGNAL( updateStars() ),
                       m_widget, SLOT( update() ) );

    m_widget->connect( m_physics, SIGNAL( valueChanged( qreal ) ),
                       m_widget, SLOT( updateAnimation( qreal ) ) );

    m_widget->connect( m_model->sunLocator(), SIGNAL( centerSun() ),
                       m_widget, SLOT( centerSun() ) );

    m_widget->setInputHandler( new MarbleWidgetDefaultInputHandler );
    m_widget->setMouseTracking( m_widget );
}

// ----------------------------------------------------------------


MarbleMap *MarbleWidget::map() const
{
    return d->m_map;
}

MarbleModel *MarbleWidget::model() const
{
    return d->m_model;
}


void MarbleWidget::setInputHandler(MarbleWidgetInputHandler *handler)
{
    delete d->m_inputhandler;
    d->m_inputhandler = handler;

    if ( d->m_inputhandler ) {
        d->m_inputhandler->init( this );
        installEventFilter( d->m_inputhandler );

        connect( d->m_inputhandler, SIGNAL( mouseClickScreenPosition( int, int) ),
                 this,              SLOT( notifyMouseClick( int, int ) ) );

        connect( d->m_inputhandler, SIGNAL( mouseMoveGeoPosition( QString ) ),
                 this,              SIGNAL( mouseMoveGeoPosition( QString ) ) );
    }
}


Quaternion MarbleWidget::planetAxis() const
{
    return d->m_map->planetAxis();
}


int MarbleWidget::radius() const
{
    return d->m_map->radius();
}

void MarbleWidget::setRadius(int radius)
{
    d->m_map->setRadius( radius );

    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    setAttribute( Qt::WA_NoSystemBackground,
                  d->m_map->mapCoversViewport() && !mapThemeId().isEmpty() );

    emit distanceChanged( distanceString() );

    repaint();
}


bool MarbleWidget::needsUpdate() const
{
    return d->m_map->needsUpdate();
}

void MarbleWidget::setNeedsUpdate()
{
    d->m_map->setNeedsUpdate();
}


QAbstractItemModel *MarbleWidget::placemarkModel() const
{
    return d->m_map->placemarkModel();
}

QItemSelectionModel *MarbleWidget::placemarkSelectionModel() const
{
    return d->m_map->placemarkSelectionModel();
}

qreal MarbleWidget::moveStep()
{
    return d->m_map->moveStep();
}

int MarbleWidget::zoom() const
{
    return d->m_map->zoom();
}

int  MarbleWidget::minimumZoom() const
{
    return d->m_map->minimumZoom();
}

int  MarbleWidget::maximumZoom() const
{
    return d->m_map->maximumZoom();
}

void MarbleWidget::addPlacemarkFile( const QString &filename )
{
    d->m_map->addPlacemarkFile( filename );
    //d->m_model->addPlacemarkFile( filename );
}

void MarbleWidget::addPlacemarkData( const QString &data, const QString &key )
{
    d->m_map->addPlacemarkData( data, key );
}

void MarbleWidget::removePlacemarkKey( const QString &key )
{
    d->m_map->removePlacemarkKey( key );
}

QPixmap MarbleWidget::mapScreenShot()
{
    return QPixmap::grabWidget( this );
}

bool MarbleWidget::showOverviewMap() const
{
    return d->m_map->showOverviewMap();
}

bool MarbleWidget::showScaleBar() const
{
    return d->m_map->showScaleBar();
}

bool MarbleWidget::showCompass() const
{
    return d->m_map->showCompass();
}

bool MarbleWidget::showClouds() const
{
    return d->m_map->showClouds();
}

bool MarbleWidget::showAtmosphere() const
{
    return d->m_map->showAtmosphere();
}

bool MarbleWidget::showCrosshairs() const
{
    return d->m_map->showCrosshairs();
}

bool MarbleWidget::showGrid() const
{
    return d->m_map->showGrid();
}

bool MarbleWidget::showPlaces() const
{
    return d->m_map->showPlaces();
}

bool MarbleWidget::showCities() const
{
    return d->m_map->showCities();
}

bool MarbleWidget::showTerrain() const
{
    return d->m_map->showTerrain();
}

bool MarbleWidget::showOtherPlaces() const
{
    return d->m_map->showOtherPlaces();
}

bool MarbleWidget::showRelief() const
{
    return d->m_map->showRelief();
}

bool MarbleWidget::showElevationModel() const
{
    return d->m_map->showElevationModel();
}

bool MarbleWidget::showIceLayer() const
{
    return d->m_map->showIceLayer();
}

bool MarbleWidget::showBorders() const
{
    return d->m_map->showBorders();
}

bool MarbleWidget::showRivers() const
{
    return d->m_map->showRivers();
}

bool MarbleWidget::showLakes() const
{
    return d->m_map->showLakes();
}

bool MarbleWidget::showGps() const
{
    return d->m_map->showGps();
}

bool MarbleWidget::showFrameRate() const
{
    return d->m_map->showFrameRate();
}

quint64 MarbleWidget::persistentTileCacheLimit() const
{
    return d->m_map->persistentTileCacheLimit();
}

quint64 MarbleWidget::volatileTileCacheLimit() const
{
    return d->m_map->volatileTileCacheLimit();
}


void MarbleWidget::zoomView(int newZoom)
{
    // This function is tricky since it needs to be possible to call
    // both from above as an ordinary function, and "from below",
    // i.e. as a slot.  That's why we need to save m_logZoom from when
    // we repainted last time.

    // Make all the internal changes to the map.
    d->m_map->zoomView( newZoom );

    // If no change, we don't need to repainting or anything else.
    if ( d->m_logZoom == newZoom )
	return;

    d->m_logZoom = newZoom;

    // We only have to repaint the background every time if the globe
    // doesn't cover the whole image.
    setAttribute( Qt::WA_NoSystemBackground,
                  d->m_map->mapCoversViewport() && !mapThemeId().isEmpty() );

    emit distanceChanged( distanceString() );

    repaint();
}


void MarbleWidget::zoomViewBy( int zoomStep )
{
    zoomView( MarbleMapPrivate::toLogScale( radius() ) + zoomStep );
}


void MarbleWidget::zoomIn()
{
    d->m_map->zoomIn();
}

void MarbleWidget::zoomOut()
{
    d->m_map->zoomOut();
}

void MarbleWidget::rotateBy(const Quaternion& incRot)
{
    d->m_map->rotateBy( incRot );

    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    setAttribute( Qt::WA_NoSystemBackground,
                  d->m_map->mapCoversViewport() && !mapThemeId().isEmpty() );

    repaint();
}

void MarbleWidget::rotateBy( const qreal& deltaLon, const qreal& deltaLat)
{
    d->m_map->rotateBy( deltaLon, deltaLat );

    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    setAttribute( Qt::WA_NoSystemBackground,
                  d->m_map->mapCoversViewport() && !mapThemeId().isEmpty() );

    repaint();
}


void MarbleWidget::centerOn( const qreal& lon, const qreal& lat, bool animated )
{
    if ( d->m_animationsEnabled && animated ) {
        d->m_physics->setCurrentPosition( GeoDataCoordinates( centerLongitude(), centerLatitude(),
                                                              distance(),
                                                              GeoDataCoordinates::Degree ) );
        d->m_physics->jumpTo( GeoDataCoordinates( lon, lat, distance(),
                                                  GeoDataCoordinates::Degree ) );
    } else {
        d->m_map->centerOn( lon, lat );
    }

    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    setAttribute( Qt::WA_NoSystemBackground,
                  d->m_map->mapCoversViewport() && !mapThemeId().isEmpty() );

    repaint();
}

void MarbleWidget::centerOn( const QModelIndex& index, bool animated )
{
    if ( d->m_animationsEnabled && animated ) {
        QItemSelectionModel *selectionModel = d->m_map->model()->placemarkSelectionModel();
        Q_ASSERT( selectionModel );
    
        selectionModel->clear();
    
        if ( index.isValid() ) {
            const GeoDataCoordinates targetPosition =
                index.data( MarblePlacemarkModel::CoordinateRole ).value<GeoDataCoordinates>();

            d->m_physics->setCurrentPosition( GeoDataCoordinates( centerLongitude(),
                                                                  centerLatitude(), distance(),
                                                                  GeoDataCoordinates::Degree ) );
            d->m_physics->jumpTo( targetPosition );

            selectionModel->select( index, QItemSelectionModel::SelectCurrent );
        }
    } else {
        d->m_map->centerOn( index );
    }

    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    setAttribute( Qt::WA_NoSystemBackground,
                  d->m_map->mapCoversViewport() && !mapThemeId().isEmpty() );

    repaint();
}

void MarbleWidget::centerOn( const GeoDataCoordinates &position, bool animated )
{
    if ( d->m_animationsEnabled && animated ) {
        GeoDataCoordinates targetPosition = position;
        targetPosition.setAltitude( distance() );

        d->m_physics->jumpTo( targetPosition );
    } else {
        qreal  lon, lat;
        position.geoCoordinates( lon, lat, GeoDataCoordinates::Degree );
        d->m_map->setDistance( position.altitude() );
        d->m_map->centerOn( lon, lat );
    }

    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    setAttribute( Qt::WA_NoSystemBackground,
                  d->m_map->mapCoversViewport() && !mapThemeId().isEmpty() );

    repaint();
}

void MarbleWidget::updateAnimation( qreal updateValue )
{
    GeoDataCoordinates position = d->m_physics->suggestedPosition();

    if ( updateValue < 1.0 ) {
        setViewContext( Marble::Animation );
        centerOn( position );
        setViewContext( Marble::Still );
        return;
    }

    centerOn( position );
}

void MarbleWidget::setCenterLatitude( qreal lat )
{
    centerOn( centerLongitude(), lat );

    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    setAttribute( Qt::WA_NoSystemBackground,
                  d->m_map->mapCoversViewport() && !mapThemeId().isEmpty() );
}

void MarbleWidget::setCenterLongitude( qreal lon )
{
    centerOn( lon, centerLatitude() );

    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    setAttribute( Qt::WA_NoSystemBackground,
                  d->m_map->mapCoversViewport() && !mapThemeId().isEmpty() );
}

Projection MarbleWidget::projection() const
{
    return d->m_map->projection();
}

void MarbleWidget::setProjection( Projection projection )
{
    d->m_map->setProjection( projection );

    setAttribute( Qt::WA_NoSystemBackground,
                  d->m_map->mapCoversViewport() && !mapThemeId().isEmpty() );

    repaint();
}

void MarbleWidget::setProjection( int projection )
{
    setProjection( (Projection)( projection ) );
}

void MarbleWidget::home( qreal &lon, qreal &lat, int& zoom )
{
    d->m_map->home( lon, lat, zoom );
}

void MarbleWidget::setHome( qreal lon, qreal lat, int zoom )
{
    d->m_map->setHome( lon, lat, zoom );
}

void MarbleWidget::setHome(const GeoDataCoordinates& homePoint, int zoom)
{
    d->m_map->setHome( homePoint, zoom );
}


void MarbleWidget::moveLeft()
{
    d->m_map->moveLeft();

    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    setAttribute( Qt::WA_NoSystemBackground,
                  d->m_map->mapCoversViewport() && !mapThemeId().isEmpty() );

    repaint();
}

void MarbleWidget::moveRight()
{
    d->m_map->moveRight();

    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    setAttribute( Qt::WA_NoSystemBackground,
                  d->m_map->mapCoversViewport() && !mapThemeId().isEmpty() );

    repaint();
}


void MarbleWidget::moveUp()
{
    d->m_map->moveUp();

    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    setAttribute( Qt::WA_NoSystemBackground,
                  d->m_map->mapCoversViewport() && !mapThemeId().isEmpty() );

    repaint();
}

void MarbleWidget::moveDown()
{
    d->m_map->moveDown();

    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    setAttribute( Qt::WA_NoSystemBackground,
                  d->m_map->mapCoversViewport() && !mapThemeId().isEmpty() );

    repaint();
}

void MarbleWidget::leaveEvent (QEvent*)
{
    emit mouseMoveGeoPosition( tr( NOT_AVAILABLE ) );
}

void MarbleWidget::resizeEvent (QResizeEvent*)
{
    setUpdatesEnabled( false );
    d->m_map->setSize( width(), height() );

    setAttribute( Qt::WA_NoSystemBackground,
                  d->m_map->mapCoversViewport() && !mapThemeId().isEmpty() );

    repaint();
    setUpdatesEnabled( true );
}

void MarbleWidget::connectNotify ( const char * signal )
{
    if ( QByteArray( signal ) == 
         QMetaObject::normalizedSignature ( SIGNAL( mouseMoveGeoPosition( QString ) ) ) )
        if ( d->m_inputhandler )
            d->m_inputhandler->setPositionSignalConnected( true );
}

void MarbleWidget::disconnectNotify ( const char * signal )
{
    if ( QByteArray( signal ) == 
         QMetaObject::normalizedSignature ( SIGNAL( mouseMoveGeoPosition( QString ) ) ) )
        if ( d->m_inputhandler )
            d->m_inputhandler->setPositionSignalConnected( false );
}

int MarbleWidget::northPoleY()
{
    return d->m_map->northPoleY();
}

bool MarbleWidget::screenCoordinates( qreal lon, qreal lat,
                                      int& x, int& y )
{
    return d->m_map->screenCoordinates( lon, lat, x, y );
}

bool MarbleWidget::geoCoordinates(int x, int y,
                                  qreal& lon, qreal& lat,
                                  GeoDataCoordinates::Unit unit )
{
    return d->m_map->geoCoordinates( x, y, lon, lat, unit );
}

qreal MarbleWidget::centerLatitude() const
{
    return d->m_map->centerLatitude();
}

qreal MarbleWidget::centerLongitude() const
{
    return d->m_map->centerLongitude();
}

const QRegion MarbleWidget::activeRegion()
{
    return d->m_map->viewParams()->currentProjection()->helper()->activeRegion();
}

const QRegion MarbleWidget::projectedRegion()
{
    return d->m_map->viewParams()->currentProjection()->helper()->projectedRegion();
}

void MarbleWidget::paintEvent(QPaintEvent *evt)
{
    QTime t;
    t.start();

    // FIXME: Better way to get the GeoPainter
    bool  doClip = true;
    if ( d->m_map->projection() == Spherical )
        doClip = ( d->m_map->radius() > width() / 2
                   || d->m_map->radius() > height() / 2 );

    // Create a painter that will do the painting.
    GeoPainter painter( this, map()->viewParams()->viewport(),
			map()->viewParams()->mapQuality(), doClip );

    QRect  dirtyRect = evt->rect();

    // Draws the map like MarbleMap::paint does, but adds our customPaint in between
    d->m_map->d->paintGround( painter, dirtyRect );
    d->m_map->customPaint( &painter );
    customPaint( &painter );
    d->m_map->d->paintOverlay( painter, dirtyRect );

    qreal fps = 1000.0 / (qreal)( t.elapsed() );
    d->m_map->d->paintFps( painter, dirtyRect, fps );
    emit d->m_map->framesPerSecond( fps );
}

void MarbleWidget::customPaint(GeoPainter *painter)
{
    Q_UNUSED( painter );
    /* This is a NOOP in the base class*/
}


void MarbleWidget::goHome()
{
    d->m_map->goHome();

    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    setAttribute( Qt::WA_NoSystemBackground,
                  d->m_map->mapCoversViewport() && !mapThemeId().isEmpty() );

    repaint(); // not obsolete in case the zoomlevel stays unaltered
}

QString MarbleWidget::mapThemeId() const
{
    return d->m_model->mapThemeId();
}

void MarbleWidget::setMapThemeId( const QString& mapThemeId )
{
    qDebug() << "MapThemeId" << mapThemeId;
    if ( !mapThemeId.isEmpty() && mapThemeId == d->m_model->mapThemeId() )
        return;

    d->m_map->setMapThemeId( mapThemeId );

    // Update texture map during the repaint that follows:
    setNeedsUpdate();
    repaint();
}

GeoSceneDocument *MarbleWidget::mapTheme() const
{
    return d->m_map->viewParams()->mapTheme();
}

void MarbleWidget::setPropertyValue( const QString& name, bool value )
{
    qDebug() << "In MarbleWidget the property " << name << "was set to " << value;
    d->m_map->setPropertyValue( name, value );

    // Update texture map during the repaint that follows:
    setNeedsUpdate();
    repaint();
}

void MarbleWidget::setShowOverviewMap( bool visible )
{
    d->m_map->setShowOverviewMap( visible );

    repaint();
}

void MarbleWidget::setShowScaleBar( bool visible )
{
    d->m_map->setShowScaleBar( visible );

    repaint();
}

void MarbleWidget::setShowCompass( bool visible )
{
    d->m_map->setShowCompass( visible );

    repaint();
}

void MarbleWidget::setShowClouds( bool visible )
{
    d->m_map->setShowClouds( visible );

    repaint();
}

void MarbleWidget::setShowAtmosphere( bool visible )
{
    d->m_map->setShowAtmosphere( visible );

    repaint();
}

void MarbleWidget::setShowCrosshairs( bool visible )
{
    d->m_map->setShowCrosshairs( visible );

    repaint();
}

void MarbleWidget::setShowGrid( bool visible )
{
    d->m_map->setShowGrid( visible );

    repaint();
}

void MarbleWidget::setShowPlaces( bool visible )
{
    d->m_map->setShowPlaces( visible );

    repaint();
}

void MarbleWidget::setShowCities( bool visible )
{
    d->m_map->setShowCities( visible );

    repaint();
}

void MarbleWidget::setShowTerrain( bool visible )
{
    d->m_map->setShowTerrain( visible );

    repaint();
}

void MarbleWidget::setShowOtherPlaces( bool visible )
{
    d->m_map->setShowOtherPlaces( visible );

    repaint();
}

void MarbleWidget::setShowRelief( bool visible )
{
    d->m_map->setShowRelief( visible );

    repaint();
}

void MarbleWidget::setShowElevationModel( bool visible )
{
    d->m_map->setShowElevationModel( visible );

    repaint();
}

void MarbleWidget::setShowIceLayer( bool visible )
{
    d->m_map->setShowIceLayer( visible );

    repaint();
}

void MarbleWidget::setShowBorders( bool visible )
{
    d->m_map->setShowBorders( visible );

    repaint();
}

void MarbleWidget::setShowRivers( bool visible )
{
    d->m_map->setShowRivers( visible );

    repaint();
}

void MarbleWidget::setShowLakes( bool visible )
{
    d->m_map->setShowLakes( visible );

    repaint();
}

void MarbleWidget::setShowFrameRate( bool visible )
{
    d->m_map->setShowFrameRate( visible );

    repaint();
}

void MarbleWidget::setShowGps( bool visible )
{
    d->m_map->setShowGps( visible );

    repaint();
}

void MarbleWidget::setShowTileId( bool visible )
{
    d->m_model->layerDecorator()->setShowTileId( visible );
}

void MarbleWidget::changeCurrentPosition( qreal lon, qreal lat)
{
    d->m_model->gpsLayer()->changeCurrentPosition( lat, lon );
    repaint();
}

void MarbleWidget::notifyMouseClick( int x, int y)
{
    qreal  lon   = 0;
    qreal  lat   = 0;

    bool const valid = geoCoordinates( x, y, lon, lat, GeoDataCoordinates::Radian );

    if ( valid ) {
        emit mouseClickGeoPosition( lon, lat, GeoDataCoordinates::Radian);
    }
}

void MarbleWidget::updateGps()
{
    QRegion temp;
    const bool draw = d->m_model->gpsLayer()->updateGps( size(),
                                                         d->m_map->viewParams(),
                                                         temp );
    if ( draw ) {
        qDebug() << "Updating viewport for GPS";
        update( temp );
    }
}

void MarbleWidget::openGpxFile(QString &filename)
{
#ifndef KML_GSOC
    d->m_model->gpsLayer()->loadGpx( filename );
#else
    GpxFileViewItem* item = new GpxFileViewItem( new GpxFile( filename ) );
    d->m_model->fileViewModel()->append( item );
#endif
}

GpxFileModel *MarbleWidget::gpxFileModel()
{
    return d->m_model->gpxFileModel();
}

FileViewModel* MarbleWidget::fileViewModel() const
{
    return d->m_model->fileViewModel();
}

void MarbleWidget::clearPersistentTileCache()
{
    d->m_map->clearPersistentTileCache();
}

void MarbleWidget::setPersistentTileCacheLimit( quint64 kiloBytes )
{
    d->m_map->setPersistentTileCacheLimit( kiloBytes );
}

void MarbleWidget::clearVolatileTileCache()
{
    qDebug() << "About to clear VolatileTileCache";
    d->m_map->clearVolatileTileCache();
}

void MarbleWidget::setVolatileTileCacheLimit( quint64 kiloBytes )
{
    d->m_map->setVolatileTileCacheLimit( kiloBytes );
}

// This slot will called when the Globe starts to create the tiles.

void MarbleWidget::creatingTilesStart( TileCreator *creator,
                                       const QString &name, 
                                       const QString &description )
{
    TileCreatorDialog dlg( creator, this );
    dlg.setSummary( name, description );
    dlg.exec();
}

void MarbleWidget::updateChangedMap()
{
    // Update texture map during the repaint that follows:
    setNeedsUpdate();
    update();
}

MapQuality MarbleWidget::mapQuality( ViewContext viewContext )
{
    if ( viewContext == Still )
        return d->m_stillQuality;

    Q_ASSERT( viewContext == Animation );
    return d->m_animationQuality; 
}

void MarbleWidget::setMapQuality( MapQuality mapQuality, ViewContext changedViewContext )
{
    // FIXME: Rewrite as a switch
    if ( changedViewContext == Still ) {
        d->m_stillQuality = mapQuality;
    }
    else if ( changedViewContext == Animation ) {
        d->m_animationQuality = mapQuality;
    }

    if ( viewContext() == Still ) {
        map()->viewParams()->setMapQuality( d->m_stillQuality ); 
    }
    else if ( viewContext() == Animation )
    {
        map()->viewParams()->setMapQuality( d->m_animationQuality ); 
    }
}

ViewContext MarbleWidget::viewContext() const
{
    return d->m_viewContext;
}

void MarbleWidget::setViewContext( Marble::ViewContext viewContext )
{
    d->m_viewContext = viewContext;

    if ( viewContext == Still )
        map()->viewParams()->setMapQuality( d->m_stillQuality ); 
    if ( viewContext == Animation )
        map()->viewParams()->setMapQuality( d->m_animationQuality ); 
}

bool MarbleWidget::animationsEnabled() const
{
    return d->m_animationsEnabled;
}

void MarbleWidget::setAnimationsEnabled( bool enabled )
{
    d->m_animationsEnabled = enabled;
}

Marble::AngleUnit MarbleWidget::defaultAngleUnit() const
{
    return map()->defaultAngleUnit();
}

void MarbleWidget::setDefaultAngleUnit( Marble::AngleUnit angleUnit )
{
    map()->setDefaultAngleUnit( angleUnit );
}

QFont MarbleWidget::defaultFont() const
{
    return map()->defaultFont();
}

void MarbleWidget::setDefaultFont( const QFont& font )
{
    map()->setDefaultFont( font );
}

void MarbleWidget::setSelection(const QRect& region)
{
    QPoint tl = region.topLeft();
    QPoint br = region.bottomRight();
    qDebug() << "Selection region: (" << tl.x() << ", " <<  tl.y() << ") (" 
             << br.x() << ", " << br.y() << ")" << endl;

    AbstractProjection *proj = d->m_map->viewParams()->currentProjection();
    GeoDataLatLonAltBox box  = proj->latLonAltBox(region, d->m_map->viewParams()->viewport());

    // NOTE: coordinates as lon1, lat1, lon2, lat2 (or West, North, East, South)
    // as left/top, right/bottom rectangle.
    QList<double> coordinates;
    coordinates << box.west(GeoDataPoint::Degree) << box.north(GeoDataPoint::Degree)
                << box.east(GeoDataPoint::Degree) << box.south(GeoDataPoint::Degree);

    qDebug() << "West: " << coordinates[0] << " North: " <<  coordinates[1]
             << " East: " << coordinates[2] << " South: " << coordinates[3] << endl;

    emit regionSelected(coordinates);
}

qreal MarbleWidget::distance() const
{
    return map()->distance();
}

void MarbleWidget::setDistance( qreal distance )
{
    map()->setDistance( distance );
}

QString MarbleWidget::distanceString() const
{
    return map()->distanceString();
}

void MarbleWidget::updateSun()
{
    // Update the sun shading.
    //SunLocator  *sunLocator = d->m_model->sunLocator();

    qDebug() << "MarbleWidget: Updating the sun shading map...";
    d->m_model->update();
    setNeedsUpdate();
    //qDebug() << "Finished updating the sun shading map";
}

void MarbleWidget::centerSun()
{
    SunLocator  *sunLocator = d->m_model->sunLocator();

    qreal  lon = sunLocator->getLon();
    qreal  lat = sunLocator->getLat();
    centerOn( lon, lat );

    disableInput();
}

SunLocator* MarbleWidget::sunLocator()
{
    return d->m_model->sunLocator();
}

void MarbleWidget::enableInput()
{
    if ( !d->m_inputhandler ) {
        setInputHandler( new MarbleWidgetDefaultInputHandler );
    }
    else {
        installEventFilter( d->m_inputhandler );
    }
}

void MarbleWidget::disableInput()
{
    qDebug() << "MarbleWidget::disableInput";
    removeEventFilter( d->m_inputhandler );
    setCursor( Qt::ArrowCursor );
}

void MarbleWidget::setProxy( const QString& proxyHost, const quint16 proxyPort, const QString& user, const QString& password  )
{
    d->m_proxyHost = proxyHost;
    d->m_proxyPort = proxyPort;
    d->m_user = user;
    d->m_password = password;

    QNetworkProxy::ProxyType type = QNetworkProxy::HttpProxy;

    // Make sure that no proxy is used for an empty string or the default value: 
    if ( proxyHost.isEmpty() || proxyHost == "http://" )
        type = QNetworkProxy::NoProxy;

    QNetworkProxy proxy( type, d->m_proxyHost, d->m_proxyPort, d->m_user, d->m_password );
    QNetworkProxy::setApplicationProxy( proxy );
    qDebug() << "MarbleWidget::setProxy" << type << d->m_proxyHost << d->m_proxyPort << d->m_user << d->m_password;
}

QString MarbleWidget::proxyHost() const
{
    return d->m_proxyHost;
}

quint16 MarbleWidget::proxyPort() const
{
    return d->m_proxyPort;
}

QString MarbleWidget::user() const
{
    return d->m_user;
}

QString MarbleWidget::password() const
{
    return d->m_password;
}

QList<RenderPlugin *> MarbleWidget::renderPlugins() const
{
    return d->m_model->renderPlugins();
}

QList<AbstractFloatItem *> MarbleWidget::floatItems() const
{
    return d->m_model->floatItems();
}

}

#include "MarbleWidget.moc"
