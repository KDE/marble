//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2010-2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "MarbleWidget.h"

#include <QtCore/qmath.h>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QHash>
#include <QtCore/QSettings>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtGui/QItemSelectionModel>
#include <QtGui/QPaintEvent>
#include <QtGui/QRegion>
#include <QtGui/QSizePolicy>
#include <QtNetwork/QNetworkProxy>

#ifdef MARBLE_DBUS
#include <QtDBus/QDBusConnection>
#endif

#include "AbstractProjection.h"
#include "DataMigration.h"
#include "FpsLayer.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoPainter.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleLocale.h"
#include "MarbleMap.h"
#include "MarbleModel.h"
#include "MarblePhysics.h"
#include "MarbleWidgetInputHandler.h"
#include "MarbleWidgetPopupMenu.h"
#include "MeasureTool.h"
#include "Planet.h"
#include "RenderPlugin.h"
#include "SunLocator.h"
#include "TileCreatorDialog.h"
#include "ViewportParams.h"
#include "routing/RoutingLayer.h"
#include "routing/RoutingManager.h"
#include "routing/AlternativeRoutesModel.h"

namespace Marble
{

const int REPAINT_SCHEDULING_INTERVAL = 1000;


class MarbleWidgetPrivate
{
 public:
    MarbleWidgetPrivate( MarbleMap *map, MarbleWidget *parent )
        : m_widget( parent ),
          m_map( map ),
          m_model( map->model() ),
          m_animationsEnabled( false ),
          m_logzoom( 0 ),
          m_zoomStep( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ? 60 : 40 ),
          m_inputhandler( 0 ),
          m_physics( new MarblePhysics( parent ) ),
          m_repaintTimer(),
          m_routingLayer( 0 ),
          m_popupmenu( 0 ),
          m_showFrameRate( false ),
          m_viewAngle( 110.0 )
    {
    }

    ~MarbleWidgetPrivate()
    {
        delete m_map;
    }

    void  construct();

    inline static qreal zoom( qreal radius ) { return (200.0 * log( radius ) ); }
    inline static qreal radius( qreal zoom ) { return pow( M_E, ( zoom / 200.0 ) ); }

    /**
      * @brief Rotate the globe in the given direction in discrete steps
      * @param stepsRight Number of steps to go right. Negative values go left.
      * @param stepsDown Number of steps to go down. Negative values go up.
      * @param mode Interpolation mode to use when traveling to the target
      */
    void moveByStep( int stepsRight, int stepsDown, FlyToMode mode );

    /**
      * @brief Update widget flags and cause a full repaint
      *
      * The background of the widget only needs to be redrawn in certain cases. This
      * method sets the widget flags accordingly and triggers a repaint.
      */
    void repaint();

    MarbleWidget    *m_widget;
    // The model we are showing.
    MarbleMap       *m_map;
    MarbleModel     *m_model;   // Owned by m_map.  Don't delete.

    bool m_animationsEnabled;

    // zoom related
    int              m_logzoom;
    int              m_zoomStep;

    MarbleWidgetInputHandler  *m_inputhandler;

    MarblePhysics    *m_physics;

    // For scheduling repaints
    QTimer           m_repaintTimer;

    RoutingLayer     *m_routingLayer;

    MarbleWidgetPopupMenu *m_popupmenu;

    bool             m_showFrameRate;

    const qreal      m_viewAngle;
};



MarbleWidget::MarbleWidget(QWidget *parent)
    : QWidget( parent ),
      d( new MarbleWidgetPrivate( new MarbleMap(), this ) )
{
//    setAttribute( Qt::WA_PaintOnScreen, true );
    d->construct();
}

MarbleWidget::~MarbleWidget()
{
    // Remove and delete an existing InputHandler
    // initialized in d->construct()
    setInputHandler( 0 );

    delete d;
}

void MarbleWidgetPrivate::construct()
{
    QPointer<DataMigration> dataMigration = new DataMigration( m_widget );
    dataMigration->exec();
    delete dataMigration;

#ifdef MARBLE_DBUS
    QDBusConnection::sessionBus().registerObject( "/MarbleWidget", m_widget,
                                                  QDBusConnection::ExportAllSlots
                                                  | QDBusConnection::ExportAllSignals
                                                  | QDBusConnection::ExportAllProperties );
#endif

    // Widget settings
    m_widget->setMinimumSize( 200, 300 );
    m_widget->setFocusPolicy( Qt::WheelFocus );
    m_widget->setFocus( Qt::OtherFocusReason );
#if QT_VERSION >= 0x40600
    m_widget->grabGesture(Qt::PinchGesture);
#endif

    // Set background: black.
    m_widget->setPalette( QPalette ( Qt::black ) );

    // Set whether the black space gets displayed or the earth gets simply
    // displayed on the widget background.
    m_widget->setAutoFillBackground( true );

    // Initialize the map and forward some signals.
    m_map->setSize( m_widget->width(), m_widget->height() );
    m_map->setShowFrameRate( false );  // never let the map draw the frame rate,
                                       // we do this differently here in the widget

    // forward some signals of m_map
    m_widget->connect( m_map,    SIGNAL( projectionChanged( Projection ) ),
                       m_widget, SIGNAL( projectionChanged( Projection ) ) );
    m_widget->connect( m_map,    SIGNAL( tileLevelChanged( int ) ),
                       m_widget, SIGNAL( tileLevelChanged( int ) ) );

    m_widget->connect( m_map,    SIGNAL( pluginSettingsChanged() ),
                       m_widget, SIGNAL( pluginSettingsChanged() ) );
    m_widget->connect( m_map,    SIGNAL( renderPluginInitialized( RenderPlugin * ) ),
                       m_widget, SIGNAL( renderPluginInitialized( RenderPlugin * ) ) );

    // react to some signals of m_map
    m_widget->connect( m_map,    SIGNAL( repaintNeeded( QRegion ) ),
                       m_widget, SLOT( scheduleRepaint( QRegion ) ) );

    // When some fundamental things change in the model, we got to
    // show this in the view, i.e. here.
    m_widget->connect( m_model,  SIGNAL( themeChanged( QString ) ),
		       m_widget, SIGNAL( themeChanged( QString ) ) );
    m_widget->connect( m_model, SIGNAL( modelChanged() ),
                       m_widget, SLOT( update() ) );

    // Show a progress dialog when the model calculates new map tiles.
    m_widget->connect( m_model, SIGNAL( creatingTilesStart( TileCreator*, const QString&,
                                                            const QString& ) ),
                       m_widget, SLOT( creatingTilesStart( TileCreator*, const QString&,
                                                           const QString& ) ) );

    m_widget->connect( m_model->sunLocator(), SIGNAL( enableWidgetInput( bool ) ),
                       m_widget, SLOT( setInputEnabled( bool ) ) );

    m_widget->connect( m_model->sunLocator(), SIGNAL( updateStars() ),
                       m_widget, SLOT( update() ) );

    m_widget->connect( m_model->sunLocator(), SIGNAL( centerSun( qreal, qreal ) ),
                       m_widget, SLOT( centerOn( qreal, qreal ) ) );

    // Repaint timer
    m_repaintTimer.setSingleShot( true );
    m_repaintTimer.setInterval( REPAINT_SCHEDULING_INTERVAL );
    m_widget->connect( &m_repaintTimer, SIGNAL( timeout() ),
                       m_widget, SLOT( update() ) );

    m_popupmenu = new MarbleWidgetPopupMenu( m_widget, m_model );

    m_widget->setInputHandler( new MarbleWidgetDefaultInputHandler( m_widget ) );
    m_widget->setMouseTracking( m_widget );

    m_routingLayer = new RoutingLayer( m_widget, m_widget );
    m_routingLayer->setRouteRequest( m_model->routingManager()->routeRequest() );
    m_routingLayer->setPlacemarkModel( 0 );
    m_map->addLayer( m_routingLayer );

    m_widget->connect( m_routingLayer, SIGNAL( routeDirty() ),
                       m_model->routingManager(), SLOT( updateRoute() ) );
    m_widget->connect( m_model->routingManager()->alternativeRoutesModel(),
                       SIGNAL( currentRouteChanged( GeoDataDocument* ) ),
                       m_widget, SLOT( repaint() ) );
}

void MarbleWidgetPrivate::moveByStep( int stepsRight, int stepsDown, FlyToMode mode )
{
    int polarity = m_widget->viewport()->polarity();
    qreal left = polarity * stepsRight * m_widget->moveStep();
    qreal down = stepsDown * m_widget->moveStep();
    m_widget->rotateBy( left, down, mode );
}

void MarbleWidgetPrivate::repaint()
{
    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    m_widget->setAttribute( Qt::WA_NoSystemBackground,
                  m_widget->viewport()->mapCoversViewport() && !m_model->mapThemeId().isEmpty() );

    m_widget->repaint();
}

// ----------------------------------------------------------------


MarbleModel *MarbleWidget::model() const
{
    return d->m_model;
}


ViewportParams* MarbleWidget::viewport()
{
    return d->m_map->viewport();
}

const ViewportParams* MarbleWidget::viewport() const
{
    return d->m_map->viewport();
}

MarbleWidgetPopupMenu *MarbleWidget::popupMenu()
{
    return d->m_popupmenu;
}


void MarbleWidget::setInputHandler( MarbleWidgetInputHandler *handler )
{
    delete d->m_inputhandler;
    d->m_inputhandler = handler;

    if ( d->m_inputhandler ) {
        installEventFilter( d->m_inputhandler );

        connect( d->m_inputhandler, SIGNAL( mouseClickScreenPosition( int, int ) ),
                 this,              SLOT( notifyMouseClick( int, int ) ) );

        connect( d->m_inputhandler, SIGNAL( mouseMoveGeoPosition( QString ) ),
                 this,              SIGNAL( mouseMoveGeoPosition( QString ) ) );
    }
}

MarbleWidgetInputHandler *MarbleWidget::inputHandler() const
{
  return d->m_inputhandler;
}

Quaternion MarbleWidget::planetAxis() const
{
    return viewport()->planetAxis();
}


int MarbleWidget::radius() const
{
    return d->m_map->radius();
}

void MarbleWidget::setRadius( int radius )
{
    Q_ASSERT( radius >= 0 );
    if ( radius == d->m_map->radius() ) {
        return;
    }

    qreal const zoom = d->zoom( radius );

    // Prevent exceeding zoom range
    if ( zoom < minimumZoom() ) {
        radius = d->radius( minimumZoom() );
    } else if ( zoom > maximumZoom() ) {
        radius = d->radius( maximumZoom() );
    }

    d->m_map->setRadius( radius );
    d->m_logzoom = qRound( zoom );

    emit zoomChanged( d->m_logzoom );
    emit distanceChanged( distanceString() );
    emit visibleLatLonAltBoxChanged( d->m_map->viewport()->viewLatLonAltBox() );

    d->repaint();
}

qreal MarbleWidget::moveStep()
{
    if ( radius() < qSqrt( (qreal)(width() * width() + height() * height()) ) )
        return 180.0 * 0.1;
    else
        return 180.0 * qAtan( (qreal)width()
                     / (qreal)( 2 * radius() ) ) * 0.2;
}

int MarbleWidget::zoom() const
{
    return d->m_logzoom;
}

int MarbleWidget::tileZoomLevel() const
{
    return d->m_map->tileZoomLevel();
}

int  MarbleWidget::minimumZoom() const
{
    return d->m_map->minimumZoom();
}

int  MarbleWidget::maximumZoom() const
{
    return d->m_map->maximumZoom();
}

QVector<const GeoDataPlacemark*> MarbleWidget::whichFeatureAt( const QPoint &curpos ) const
{
    return d->m_map->whichFeatureAt( curpos );
}

QList<AbstractDataPluginItem*> MarbleWidget::whichItemAt( const QPoint &curpos ) const
{
    return d->m_map->whichItemAt( curpos );
}

void MarbleWidget::addLayer( LayerInterface *layer )
{
    d->m_map->addLayer( layer );
}

void MarbleWidget::removeLayer( LayerInterface *layer )
{
    d->m_map->removeLayer( layer );
}

TextureLayer *MarbleWidget::textureLayer()
{
    return d->m_map->textureLayer();
}

const Marble::TextureLayer* MarbleWidget::textureLayer() const
{
    return d->m_map->textureLayer();
}

MeasureTool *MarbleWidget::measureTool()
{
    return d->m_map->measureTool();
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
    return d->m_showFrameRate;
}

bool MarbleWidget::showBackground() const
{
    return d->m_map->showBackground();
}

quint64 MarbleWidget::volatileTileCacheLimit() const
{
    return d->m_map->volatileTileCacheLimit();
}


void MarbleWidget::zoomView( int newZoom, FlyToMode mode )
{
    // It won't fly anyway. So we should do everything to keep the zoom value.
    if ( !d->m_animationsEnabled || mode == Instant ) {
        // Check for under and overflow.
        if ( newZoom < minimumZoom() )
            newZoom = minimumZoom();
        else if ( newZoom > maximumZoom() )
            newZoom = maximumZoom();

        // Prevent infinite loops.
        if ( newZoom  == d->m_logzoom )
            return;

        d->m_map->setRadius( d->radius( newZoom ) );
        d->m_logzoom = newZoom;

        emit zoomChanged( d->m_logzoom );
        emit distanceChanged( distanceString() );
        emit visibleLatLonAltBoxChanged( d->m_map->viewport()->viewLatLonAltBox() );

        d->repaint();
    }
    else {
        GeoDataLookAt target = lookAt();
        target.setRange( KM2METER * distanceFromZoom( newZoom ) );

        flyTo( target, mode );
    }
}


void MarbleWidget::zoomViewBy( int zoomStep, FlyToMode mode )
{
    zoomView( zoom() + zoomStep, mode );
}


void MarbleWidget::zoomIn( FlyToMode mode )
{
    if ( d->m_map->tileZoomLevel() < 0 ) {
        zoomViewBy( d->m_zoomStep, mode );
    } else {
        int radius = d->m_map->preferredRadiusCeil( d->m_map->radius() * 1.05 ) + 2;
        radius = qMax<int>( d->radius( minimumZoom() ), qMin<int>( radius, d->radius( maximumZoom() ) ) );

        GeoDataLookAt target = lookAt();
        target.setRange( KM2METER * distanceFromRadius( radius ) );

        flyTo( target, mode );
    }
}

void MarbleWidget::zoomOut( FlyToMode mode )
{
    if ( d->m_map->tileZoomLevel() < 0 ) {
        zoomViewBy( -d->m_zoomStep, mode );
    } else {
        int radius = d->m_map->preferredRadiusFloor( d->m_map->radius() * 0.95 ) + 2;
        radius = qMax<int>( d->radius( minimumZoom() ), qMin<int>( radius, d->radius( maximumZoom() ) ) );

        GeoDataLookAt target = lookAt();
        target.setRange( KM2METER * distanceFromRadius( radius ) );

        flyTo( target, mode );
    }
}

void MarbleWidget::rotateBy( const Quaternion& incRot )
{
    d->m_map->rotateBy( incRot );
    d->repaint();
}

void MarbleWidget::rotateBy( const qreal deltaLon, const qreal deltaLat, FlyToMode mode )
{
    Quaternion  rotPhi( 1.0, deltaLat / 180.0, 0.0, 0.0 );
    Quaternion  rotTheta( 1.0, 0.0, deltaLon / 180.0, 0.0 );

    Quaternion  axis = planetAxis();
    qreal lon( 0.0 ), lat( 0.0 );
    axis.getSpherical( lon, lat );
    axis = rotTheta * axis;
    axis *= rotPhi;
    axis.normalize();
    lat = -axis.pitch();
    lon = axis.yaw();
    
    GeoDataLookAt target = lookAt();
    target.setLongitude( lon );
    target.setLatitude( lat );
    flyTo( target, mode );
}


void MarbleWidget::centerOn( const qreal lon, const qreal lat, bool animated )
{
    GeoDataCoordinates target( lon, lat, 0.0, GeoDataCoordinates::Degree );
    centerOn( target, animated );
}

void MarbleWidget::centerOn( const GeoDataCoordinates &position, bool animated )
{
    GeoDataLookAt target = lookAt();
    target.setCoordinates( position );
    flyTo( target, animated ? Automatic : Instant );
}

void MarbleWidget::centerOn( const GeoDataLatLonBox &box, bool animated )
{
    int newRadius = radius();
    ViewportParams* viewparams = viewport();
    //prevent divide by zero
    if( box.height() && box.width() ) {
        //work out the needed zoom level
        int const horizontalRadius = ( 0.25 * M_PI ) * ( viewparams->height() / box.height() );
        int const verticalRadius = ( 0.25 * M_PI ) * ( viewparams->width() / box.width() );
        newRadius = qMin<int>( horizontalRadius, verticalRadius );
        newRadius = qMax<int>( d->radius( minimumZoom() ), qMin<int>( newRadius, d->radius( maximumZoom() ) ) );
    }

    //move the map
    GeoDataLookAt target;
    target.setCoordinates( box.center() );
    target.setAltitude( box.center().altitude() );
    target.setRange(KM2METER * distanceFromRadius( newRadius ));
    flyTo( target, animated ? Automatic : Instant );
}

void MarbleWidget::centerOn( const GeoDataPlacemark& placemark, bool animated )
{
    GeoDataLookAt *lookAt( placemark.lookAt() );
    if ( lookAt ) {
        flyTo( *lookAt, animated ? Automatic : Instant );
    } else {
        centerOn( placemark.geometry()->latLonAltBox(), animated );
    }
}

void MarbleWidget::setCenterLatitude( qreal lat, FlyToMode mode )
{
    centerOn( centerLongitude(), lat, mode );
}

void MarbleWidget::setCenterLongitude( qreal lon, FlyToMode mode )
{
    centerOn( lon, centerLatitude(), mode );
}

Projection MarbleWidget::projection() const
{
    return d->m_map->projection();
}

void MarbleWidget::setProjection( Projection projection )
{
    d->m_map->setProjection( projection );
    d->repaint();
}

void MarbleWidget::setProjection( int projection )
{
    setProjection( (Projection)( projection ) );
}

void MarbleWidget::moveLeft( FlyToMode mode )
{
    d->moveByStep( -1, 0, mode );
}

void MarbleWidget::moveRight( FlyToMode mode )
{
    d->moveByStep( 1, 0, mode );
}


void MarbleWidget::moveUp( FlyToMode mode )
{
    d->moveByStep( 0, -1, mode );
}

void MarbleWidget::moveDown( FlyToMode mode )
{
    d->moveByStep( 0, 1, mode );
}

void MarbleWidget::leaveEvent( QEvent* )
{
    emit mouseMoveGeoPosition( tr( NOT_AVAILABLE ) );
}

void MarbleWidget::resizeEvent( QResizeEvent* )
{
    setUpdatesEnabled( false );
    d->m_map->setSize( width(), height() );
    d->repaint();
    setUpdatesEnabled( true );
}

void MarbleWidget::connectNotify( const char * signal )
{
    if ( QByteArray( signal ) == 
         QMetaObject::normalizedSignature ( SIGNAL( mouseMoveGeoPosition( QString ) ) ) )
        if ( d->m_inputhandler )
            d->m_inputhandler->setPositionSignalConnected( true );
}

void MarbleWidget::disconnectNotify( const char * signal )
{
    if ( QByteArray( signal ) == 
         QMetaObject::normalizedSignature ( SIGNAL( mouseMoveGeoPosition( QString ) ) ) )
        if ( d->m_inputhandler )
            d->m_inputhandler->setPositionSignalConnected( false );
}

bool MarbleWidget::screenCoordinates( qreal lon, qreal lat,
                                      qreal& x, qreal& y )
{
    return d->m_map->screenCoordinates( lon, lat, x, y );
}

bool MarbleWidget::geoCoordinates( int x, int y,
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

QRegion MarbleWidget::activeRegion()
{
    return viewport()->activeRegion();
}

QRegion MarbleWidget::mapRegion()
{
    return viewport()->currentProjection()->mapRegion( viewport() );
}

void MarbleWidget::paintEvent( QPaintEvent *evt )
{
    // Stop repaint timer if it is already running
    d->m_repaintTimer.stop();
    QTime t;
    t.start();

    // FIXME: Better way to get the GeoPainter
    bool  doClip = true;
    if ( projection() == Spherical )
        doClip = ( radius() > width() / 2
                   || radius() > height() / 2 );

    QPaintDevice *paintDevice = this;
    QImage image;
    if (!isEnabled())
    {
        // If the globe covers fully the screen then we can use the faster
        // RGB32 as there are no translucent areas involved.
        QImage::Format imageFormat = ( d->m_map->mapCoversViewport() )
                                     ? QImage::Format_RGB32
                                     : QImage::Format_ARGB32_Premultiplied;
        // Paint to an intermediate image
        image = QImage( rect().size(), imageFormat );
        image.fill( Qt::transparent );
        paintDevice = &image;
    }

    // Create a painter that will do the painting.
    GeoPainter painter( paintDevice, d->m_map->viewport(),
                        d->m_map->mapQuality(), doClip );
    QRect  dirtyRect = evt->rect();

    // Draws the map like MarbleMap::paint does, but adds our customPaint in between
    d->m_map->paint( painter, dirtyRect );
    customPaint( &painter );

    if ( !isEnabled() )
    {
        // Draw a grayscale version of the intermediate image
        QRgb* pixel = reinterpret_cast<QRgb*>( image.scanLine( 0 ));
        for (int i=0; i<image.width()*image.height(); ++i, ++pixel) {
            int gray = qGray( *pixel );
            *pixel = qRgb( gray, gray, gray );
        }

        GeoPainter widgetPainter( this, d->m_map->viewport(),
                            d->m_map->mapQuality(), doClip );
        widgetPainter.drawImage( rect(), image );
    }

    if ( d->m_showFrameRate )
    {
        FpsLayer fpsLayer( &t );
        fpsLayer.render( &painter, d->m_map->viewport() );

        const qreal fps = 1000.0 / (qreal)( t.elapsed() + 1 );
        emit framesPerSecond( fps );
    }
}

void MarbleWidget::customPaint( GeoPainter *painter )
{
    Q_UNUSED( painter );
    /* This is a NOOP in the base class*/
}


void MarbleWidget::goHome( FlyToMode mode )
{
    qreal  homeLon = 0;
    qreal  homeLat = 0;
    int homeZoom = 0;
    d->m_model->home( homeLon, homeLat, homeZoom );

    GeoDataLookAt target;
    target.setLongitude( homeLon, GeoDataCoordinates::Degree );
    target.setLatitude( homeLat, GeoDataCoordinates::Degree );
    target.setRange( 1000 * distanceFromZoom( homeZoom ) );

    flyTo( target, mode );
}

QString MarbleWidget::mapThemeId() const
{
    return d->m_model->mapThemeId();
}

void MarbleWidget::setMapThemeId( const QString& mapThemeId )
{
    if ( !mapThemeId.isEmpty() && mapThemeId == d->m_model->mapThemeId() )
        return;
    
    d->m_map->setMapThemeId( mapThemeId );

    // Now we want a full repaint as the atmosphere might differ
    setAttribute( Qt::WA_NoSystemBackground,
                  false );

    SunLocator  *sunLocator = d->m_model->sunLocator();

    if ( sunLocator && sunLocator->getCentered() ) {
        qreal  lon = sunLocator->getLon();
        qreal  lat = sunLocator->getLat();
        centerOn( lon, lat );

        setInputEnabled( false );
    }

    repaint();
}

GeoSceneDocument *MarbleWidget::mapTheme() const
{
    return d->m_model->mapTheme();
}

void MarbleWidget::setPropertyValue( const QString& name, bool value )
{
    mDebug() << "In MarbleWidget the property " << name << "was set to " << value;
    d->m_map->setPropertyValue( name, value );

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
    d->m_showFrameRate = visible;

    repaint();
}

void MarbleWidget::setShowBackground( bool visible )
{
    d->m_map->setShowBackground( visible );

    repaint();
}

void MarbleWidget::setShowGps( bool visible )
{
    d->m_map->setShowGps( visible );

    repaint();
}

void MarbleWidget::setShowTileId( bool visible )
{
    d->m_map->setShowTileId( visible );
}

void MarbleWidget::notifyMouseClick( int x, int y)
{
    qreal  lon   = 0;
    qreal  lat   = 0;

    bool const valid = geoCoordinates( x, y, lon, lat, GeoDataCoordinates::Radian );

    if ( valid ) {
        emit mouseClickGeoPosition( lon, lat, GeoDataCoordinates::Radian );
    }
}

void MarbleWidget::clearVolatileTileCache()
{
    mDebug() << "About to clear VolatileTileCache";
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

void MarbleWidget::scheduleRepaint( const QRegion& dirtyRegion )
{
    Q_UNUSED( dirtyRegion );
    if ( !d->m_repaintTimer.isActive() ) {
        d->m_repaintTimer.start();
    }
}

MapQuality MarbleWidget::mapQuality( ViewContext viewContext )
{
    return d->m_map->mapQuality( viewContext );
}

void MarbleWidget::setMapQualityForViewContext( MapQuality quality, ViewContext viewContext )
{
    const MapQuality oldQuality = d->m_map->mapQuality();

    d->m_map->setMapQualityForViewContext( quality, viewContext );

    if ( d->m_map->mapQuality() != oldQuality )
        d->repaint();
}

ViewContext MarbleWidget::viewContext() const
{
    return d->m_map->viewContext();
}

void MarbleWidget::setViewContext( ViewContext viewContext )
{
    const MapQuality oldQuality = d->m_map->mapQuality();

    d->m_map->setViewContext( viewContext );

    if ( d->m_map->mapQuality() != oldQuality )
        d->repaint();
}

bool MarbleWidget::animationsEnabled() const
{
    return d->m_animationsEnabled;
}

void MarbleWidget::setAnimationsEnabled( bool enabled )
{
    d->m_animationsEnabled = enabled;
}

AngleUnit MarbleWidget::defaultAngleUnit() const
{
    return d->m_map->defaultAngleUnit();
}

void MarbleWidget::setDefaultAngleUnit( AngleUnit angleUnit )
{
    d->m_map->setDefaultAngleUnit( angleUnit );
}

QFont MarbleWidget::defaultFont() const
{
    return d->m_map->defaultFont();
}

void MarbleWidget::setDefaultFont( const QFont& font )
{
    d->m_map->setDefaultFont( font );
}

void MarbleWidget::setSelection( const QRect& region )
{
    QPoint tl = region.topLeft();
    QPoint br = region.bottomRight();
    mDebug() << "Selection region: (" << tl.x() << ", " <<  tl.y() << ") (" 
             << br.x() << ", " << br.y() << ")" << endl;

    const AbstractProjection *proj = viewport()->currentProjection();
    GeoDataLatLonAltBox box  = proj->latLonAltBox( region, viewport() );

    // NOTE: coordinates as lon1, lat1, lon2, lat2 (or West, North, East, South)
    // as left/top, right/bottom rectangle.
    QList<double> coordinates;
    coordinates << box.west( GeoDataPoint::Degree ) << box.north( GeoDataPoint::Degree )
                << box.east( GeoDataPoint::Degree ) << box.south( GeoDataPoint::Degree );

    mDebug() << "West: " << coordinates[0] << " North: " <<  coordinates[1]
             << " East: " << coordinates[2] << " South: " << coordinates[3] << endl;

    emit regionSelected( coordinates );
}

qreal MarbleWidget::distance() const
{
    return distanceFromRadius(radius());
}

void MarbleWidget::setDistance( qreal newDistance )
{
    qreal minDistance = 0.001;

    if ( newDistance <= minDistance ) {
        mDebug() << "Invalid distance: 0 m";
        newDistance = minDistance;
    }    

    int newRadius = radiusFromDistance( newDistance );
    setRadius( newRadius );
}

QString MarbleWidget::distanceString() const
{
    qreal dist = distance();
    QString distanceUnitString;

    const DistanceUnit distanceUnit = MarbleGlobal::getInstance()->locale()->distanceUnit();

    if ( distanceUnit == Meter ) {
        distanceUnitString = tr("km");
    }
    else {
        dist *= KM2MI;
        distanceUnitString = tr("mi");
    }

    return QString( "%L1 %2" ).arg( dist, 8, 'f', 1, QChar(' ') ).arg( distanceUnitString );
}

void MarbleWidget::setInputEnabled( bool enabled )
{
    //if input is set as enabled
    if ( enabled )
    {
        if ( !d->m_inputhandler ) {
            setInputHandler( new MarbleWidgetDefaultInputHandler( this ) );
        }
        else {
            installEventFilter( d->m_inputhandler );
        }
    }

    else // input is disabled
    {
        mDebug() << "MarbleWidget::disableInput";
        removeEventFilter( d->m_inputhandler );
        setCursor( Qt::ArrowCursor );
    }
}

QList<RenderPlugin *> MarbleWidget::renderPlugins() const
{
    return d->m_map->renderPlugins();
}

void MarbleWidget::readPluginSettings( QSettings& settings )
{
    foreach( RenderPlugin *plugin, renderPlugins() ) {
        settings.beginGroup( QString( "plugin_" ) + plugin->nameId() );

        QHash<QString,QVariant> hash = plugin->settings();

        foreach ( const QString& key, settings.childKeys() ) {
            hash.insert( key, settings.value( key ) );
        }

        plugin->setSettings( hash );

        settings.endGroup();
    }
}

void MarbleWidget::writePluginSettings( QSettings& settings ) const
{
    foreach( RenderPlugin *plugin, renderPlugins() ) {
        settings.beginGroup( QString( "plugin_" ) + plugin->nameId() );

        QHash<QString,QVariant> hash = plugin->settings();

        QHash<QString,QVariant>::iterator it = hash.begin();
        while( it != hash.end() ) {
            settings.setValue( it.key(), it.value() );
            ++it;
        }

        settings.endGroup();
    }
}

QList<AbstractFloatItem *> MarbleWidget::floatItems() const
{
    return d->m_map->floatItems();
}

AbstractFloatItem * MarbleWidget::floatItem( const QString &nameId ) const
{
    return d->m_map->floatItem( nameId );
}

void MarbleWidget::changeEvent( QEvent * event )
{
    if ( event->type() == QEvent::EnabledChange )
    {
        setInputEnabled(isEnabled());
    }

    QWidget::changeEvent(event);
}

void MarbleWidget::flyTo( const GeoDataLookAt &newLookAt, FlyToMode mode )
{
    if ( !d->m_animationsEnabled || mode == Instant ) {
        const int radius = qRound( radiusFromDistance( newLookAt.range() * METER2KM ) );
        qreal const zoom = d->zoom( radius );
        // Prevent exceeding zoom range. Note: Bounding to range is not useful here
        if ( zoom >= minimumZoom() && zoom <= maximumZoom() ) {
            d->m_map->setRadius( radius );
            d->m_logzoom = qRound( d->zoom( radius ) );

            GeoDataCoordinates::Unit deg = GeoDataCoordinates::Degree;
            d->m_map->centerOn( newLookAt.longitude( deg ), newLookAt.latitude( deg ) );

            emit zoomChanged( d->m_logzoom );
            emit distanceChanged( distanceString() );
            emit visibleLatLonAltBoxChanged( d->m_map->viewport()->viewLatLonAltBox() );

            d->repaint();
        }
    }
    else {
        d->m_physics->flyTo( newLookAt, mode );
    }
}

void MarbleWidget::reloadMap()
{
    d->m_map->reload();
}

void MarbleWidget::downloadRegion( QString const & sourceDir, QVector<TileCoordsPyramid> const & pyramid )
{
    d->m_map->downloadRegion( sourceDir, pyramid );
}

GeoDataLookAt MarbleWidget::lookAt() const
{
    GeoDataLookAt result;
    qreal lon( 0.0 ), lat( 0.0 );

    d->m_map->viewport()->centerCoordinates( lon, lat );
    result.setLongitude( lon );
    result.setLatitude( lat );
    result.setAltitude( 0.0 );
    result.setRange( distance() * KM2METER );

    return result;
}

qreal MarbleWidget::radiusFromDistance( qreal distance ) const
{
    return  model()->planet()->radius() /
            ( distance * tan( 0.5 * d->m_viewAngle * DEG2RAD ) / 0.4 );
}

qreal MarbleWidget::distanceFromRadius( qreal radius ) const
{
    // Due to Marble's orthographic projection ("we have no focus")
    // it's actually not possible to calculate a "real" distance.
    // Additionally the viewing angle of the earth doesn't adjust to
    // the window's size.
    //
    // So the only possible workaround is to come up with a distance
    // definition which gives a reasonable approximation of
    // reality. Therefore we assume that the average window width
    // (about 800 pixels) equals the viewing angle of a human being.

    return ( model()->planet()->radius() * 0.4
            / radius / tan( 0.5 * d->m_viewAngle * DEG2RAD ) );
}

qreal MarbleWidget::zoomFromDistance( qreal distance ) const
{
    return d->zoom( radiusFromDistance( distance ) );
}

qreal MarbleWidget::distanceFromZoom( qreal zoom ) const
{
    return distanceFromRadius( d->radius( zoom ) );
}

RoutingLayer* MarbleWidget::routingLayer()
{
    return d->m_routingLayer;
}

}

#include "MarbleWidget.moc"
