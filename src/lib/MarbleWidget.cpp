//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "MarbleWidget.h"

#include <QtCore/qmath.h>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QHash>
#include <QtCore/QSettings>
#include <QtCore/QTime>
#include <QtGui/QItemSelectionModel>
#include <QtGui/QPaintEvent>
#include <QtGui/QRegion>
#include <QtGui/QSizePolicy>
#include <QtNetwork/QNetworkProxy>

#ifdef MARBLE_DBUS
#include <QtDBus/QDBusConnection>
#endif

#include "DataMigration.h"
#include "FpsLayer.h"
#include "FileManager.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataPlacemark.h"
#include "GeoPainter.h"
#include "MarbleClock.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleLocale.h"
#include "MarbleMap.h"
#include "MarbleModel.h"
#include "MarblePhysics.h"
#include "MarbleWidgetInputHandler.h"
#include "MarbleWidgetPopupMenu.h"
#include "Planet.h"
#include "RenderPlugin.h"
#include "SunLocator.h"
#include "TileCreatorDialog.h"
#include "ViewportParams.h"
#include "routing/RoutingLayer.h"
#include "routing/RoutingManager.h"
#include "routing/AlternativeRoutesModel.h"
#include "MapInfoDialog.h"

namespace Marble
{

class MarbleWidget::CustomPaintLayer : public LayerInterface
{
 public:
    CustomPaintLayer( MarbleWidget *widget )
        : m_widget( widget )
    {
    }

    virtual QStringList renderPosition() const { return QStringList() << "USER_TOOLS"; }

    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
                         const QString &renderPos, GeoSceneLayer *layer )
    {
        Q_UNUSED( viewport );
        Q_UNUSED( renderPos );
        Q_UNUSED( layer );

        m_widget->customPaint( painter );

        return true;
    }

    virtual qreal zValue() const { return 1.0e7; }

 private:
    MarbleWidget *const m_widget;
};


class MarbleWidgetPrivate
{
 public:
    MarbleWidgetPrivate( MarbleWidget *parent )
        : m_widget( parent ),
          m_model(),
          m_map( &m_model ),
          m_animationsEnabled( false ),
          m_logzoom( 0 ),
          m_zoomStep( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ? 60 : 40 ),
          m_inputhandler( 0 ),
          m_physics( parent ),
          m_routingLayer( 0 ),
          m_mapInfoDialog( 0 ),
          m_customPaintLayer( parent ),
          m_popupmenu( 0 ),
          m_showFrameRate( false ),
          m_viewAngle( 110.0 )
    {
    }

    ~MarbleWidgetPrivate()
    {
        m_map.removeLayer( &m_customPaintLayer );
    }

    void  construct();

    void updateMapTheme();

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
    void updateSystemBackgroundAttribute();

    MarbleWidget    *const m_widget;
    // The model we are showing.
    MarbleModel     m_model;
    MarbleMap       m_map;

    bool m_animationsEnabled;

    // zoom related
    int              m_logzoom;
    int              m_zoomStep;

    MarbleWidgetInputHandler  *m_inputhandler;

    MarblePhysics    m_physics;

    RoutingLayer     *m_routingLayer;
    MapInfoDialog    *m_mapInfoDialog;
    MarbleWidget::CustomPaintLayer m_customPaintLayer;

    MarbleWidgetPopupMenu *m_popupmenu;

    bool             m_showFrameRate;

    const qreal      m_viewAngle;
};



MarbleWidget::MarbleWidget(QWidget *parent)
    : QWidget( parent ),
      d( new MarbleWidgetPrivate( this ) )
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

    // Set background: black.
    m_widget->setPalette( QPalette ( Qt::black ) );

    // Set whether the black space gets displayed or the earth gets simply
    // displayed on the widget background.
    m_widget->setAutoFillBackground( true );

    // Initialize the map and forward some signals.
    m_map.setSize( m_widget->width(), m_widget->height() );
    m_map.setShowFrameRate( false );  // never let the map draw the frame rate,
                                       // we do this differently here in the widget

    // forward some signals of m_map
    m_widget->connect( &m_map,   SIGNAL( visibleLatLonAltBoxChanged( const GeoDataLatLonAltBox & ) ),
                       m_widget, SIGNAL( visibleLatLonAltBoxChanged( const GeoDataLatLonAltBox & ) ) );
    m_widget->connect( &m_map,   SIGNAL( projectionChanged( Projection ) ),
                       m_widget, SIGNAL( projectionChanged( Projection ) ) );
    m_widget->connect( &m_map,   SIGNAL( tileLevelChanged( int ) ),
                       m_widget, SIGNAL( tileLevelChanged( int ) ) );
    m_widget->connect( &m_map,   SIGNAL( framesPerSecond( qreal ) ),
                       m_widget, SIGNAL( framesPerSecond( qreal ) ) );

    m_widget->connect( &m_map,   SIGNAL( pluginSettingsChanged() ),
                       m_widget, SIGNAL( pluginSettingsChanged() ) );
    m_widget->connect( &m_map,   SIGNAL( renderPluginInitialized( RenderPlugin * ) ),
                       m_widget, SIGNAL( renderPluginInitialized( RenderPlugin * ) ) );

    // react to some signals of m_map
    m_widget->connect( &m_map,   SIGNAL( themeChanged( QString ) ),
                       m_widget, SLOT( updateMapTheme() ) );
    m_widget->connect( &m_map,   SIGNAL( repaintNeeded( QRegion ) ),
                       m_widget, SLOT( update() ) );
    m_widget->connect( &m_map,   SIGNAL( visibleLatLonAltBoxChanged( const GeoDataLatLonAltBox & ) ),
                       m_widget, SLOT( updateSystemBackgroundAttribute() ) );

    m_widget->connect( m_model.fileManager(), SIGNAL( centeredDocument(GeoDataLatLonBox) ),
                       m_widget, SLOT( centerOn(GeoDataLatLonBox) ) );


    // Show a progress dialog when the model calculates new map tiles.
    m_widget->connect( &m_model, SIGNAL( creatingTilesStart( TileCreator*, const QString&,
                                                             const QString& ) ),
                       m_widget, SLOT( creatingTilesStart( TileCreator*, const QString&,
                                                           const QString& ) ) );

    m_popupmenu = new MarbleWidgetPopupMenu( m_widget, &m_model );

    m_widget->connect( m_popupmenu, SIGNAL( trackPlacemark( const GeoDataPlacemark* ) ),
                       &m_model, SLOT( setTrackedPlacemark( const GeoDataPlacemark* ) ) );

    m_routingLayer = new RoutingLayer( m_widget, m_widget );
    m_routingLayer->setPlacemarkModel( 0 );

    m_mapInfoDialog = new MapInfoDialog( m_widget );
    m_mapInfoDialog->setVisible( false );
    m_widget->connect( m_mapInfoDialog, SIGNAL( repaintNeeded() ), m_widget, SLOT( update() ) );
    m_map.addLayer( m_mapInfoDialog );

    m_widget->setInputHandler( new MarbleWidgetDefaultInputHandler( m_widget ) );
    m_widget->setMouseTracking( true );

    m_widget->connect( m_routingLayer, SIGNAL( routeDirty() ),
                       m_model.routingManager(), SLOT( retrieveRoute() ) );
    m_widget->connect( m_model.routingManager()->alternativeRoutesModel(),
                       SIGNAL( currentRouteChanged( GeoDataDocument* ) ),
                       m_widget, SLOT( repaint() ) );

    m_map.addLayer( &m_customPaintLayer );
}

void MarbleWidgetPrivate::moveByStep( int stepsRight, int stepsDown, FlyToMode mode )
{
    int polarity = m_widget->viewport()->polarity();
    qreal left = polarity * stepsRight * m_widget->moveStep();
    qreal down = stepsDown * m_widget->moveStep();
    m_widget->rotateBy( left, down, mode );
}

void MarbleWidgetPrivate::updateSystemBackgroundAttribute()
{
    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    const bool isOn = m_map.viewport()->mapCoversViewport() && !m_model.mapThemeId().isEmpty();
    m_widget->setAttribute( Qt::WA_NoSystemBackground, isOn );
}

// ----------------------------------------------------------------


MarbleModel *MarbleWidget::model() const
{
    return &d->m_model;
}


ViewportParams* MarbleWidget::viewport()
{
    return d->m_map.viewport();
}

const ViewportParams* MarbleWidget::viewport() const
{
    return d->m_map.viewport();
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

int MarbleWidget::radius() const
{
    return d->m_map.radius();
}

void MarbleWidget::setRadius( int radius )
{
    Q_ASSERT( radius >= 0 );
    bool adjustRadius = radius != d->m_map.radius();

    qreal const zoom = d->zoom( radius );

    // Prevent exceeding zoom range
    if ( zoom < minimumZoom() ) {
        radius = d->radius( minimumZoom() );
        adjustRadius = true;
    } else if ( zoom > maximumZoom() ) {
        radius = d->radius( maximumZoom() );
        adjustRadius = true;
    }

    if( adjustRadius) {
        d->m_map.setRadius( radius );
        d->m_logzoom = qRound( zoom );

        emit zoomChanged( d->m_logzoom );
        emit distanceChanged( distanceString() );

        update();
    }
}

qreal MarbleWidget::moveStep() const
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
    return d->m_map.tileZoomLevel();
}

int  MarbleWidget::minimumZoom() const
{
    return d->m_map.minimumZoom();
}

int  MarbleWidget::maximumZoom() const
{
    return d->m_map.maximumZoom();
}

QVector<const GeoDataPlacemark*> MarbleWidget::whichFeatureAt( const QPoint &curpos ) const
{
    return d->m_map.whichFeatureAt( curpos );
}

QList<AbstractDataPluginItem*> MarbleWidget::whichItemAt( const QPoint &curpos ) const
{
    return d->m_map.whichItemAt( curpos );
}

void MarbleWidget::addLayer( LayerInterface *layer )
{
    d->m_map.addLayer( layer );
}

void MarbleWidget::removeLayer( LayerInterface *layer )
{
    d->m_map.removeLayer( layer );
}

const Marble::TextureLayer* MarbleWidget::textureLayer() const
{
    return d->m_map.textureLayer();
}

QPixmap MarbleWidget::mapScreenShot()
{
    return QPixmap::grabWidget( this );
}

bool MarbleWidget::showOverviewMap() const
{
    return d->m_map.showOverviewMap();
}

bool MarbleWidget::showScaleBar() const
{
    return d->m_map.showScaleBar();
}

bool MarbleWidget::showCompass() const
{
    return d->m_map.showCompass();
}

bool MarbleWidget::showClouds() const
{
    return d->m_map.showClouds();
}

bool MarbleWidget::showSunShading() const
{
    return d->m_map.showSunShading();
}

bool MarbleWidget::showCityLights() const
{
    return d->m_map.showCityLights();
}

bool MarbleWidget::isLockedToSubSolarPoint() const
{
    return d->m_map.isLockedToSubSolarPoint();
}

bool MarbleWidget::isSubSolarPointIconVisible() const
{
    return d->m_map.isSubSolarPointIconVisible();
}

bool MarbleWidget::showAtmosphere() const
{
    return d->m_map.showAtmosphere();
}

bool MarbleWidget::showCrosshairs() const
{
    return d->m_map.showCrosshairs();
}

bool MarbleWidget::showGrid() const
{
    return d->m_map.showGrid();
}

bool MarbleWidget::showPlaces() const
{
    return d->m_map.showPlaces();
}

bool MarbleWidget::showCities() const
{
    return d->m_map.showCities();
}

bool MarbleWidget::showTerrain() const
{
    return d->m_map.showTerrain();
}

bool MarbleWidget::showOtherPlaces() const
{
    return d->m_map.showOtherPlaces();
}

bool MarbleWidget::showRelief() const
{
    return d->m_map.showRelief();
}

bool MarbleWidget::showIceLayer() const
{
    return d->m_map.showIceLayer();
}

bool MarbleWidget::showBorders() const
{
    return d->m_map.showBorders();
}

bool MarbleWidget::showRivers() const
{
    return d->m_map.showRivers();
}

bool MarbleWidget::showLakes() const
{
    return d->m_map.showLakes();
}

bool MarbleWidget::showFrameRate() const
{
    return d->m_showFrameRate;
}

bool MarbleWidget::showBackground() const
{
    return d->m_map.showBackground();
}

quint64 MarbleWidget::volatileTileCacheLimit() const
{
    return d->m_map.volatileTileCacheLimit();
}


void MarbleWidget::setZoom( int newZoom, FlyToMode mode )
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

        d->m_map.setRadius( d->radius( newZoom ) );
        d->m_logzoom = newZoom;

        emit zoomChanged( d->m_logzoom );
        emit distanceChanged( distanceString() );

        update();
    }
    else {
        GeoDataLookAt target = lookAt();
        target.setRange( KM2METER * distanceFromZoom( newZoom ) );

        flyTo( target, mode );
    }
}

void MarbleWidget::zoomView( int zoom, FlyToMode mode )
{
    setZoom( zoom, mode );
}


void MarbleWidget::zoomViewBy( int zoomStep, FlyToMode mode )
{
    setZoom( zoom() + zoomStep, mode );
}


void MarbleWidget::zoomIn( FlyToMode mode )
{
    if ( d->m_map.tileZoomLevel() < 0 ) {
        zoomViewBy( d->m_zoomStep, mode );
    } else {
        int radius = d->m_map.preferredRadiusCeil( d->m_map.radius() * 1.05 );
        radius = qMax<int>( d->radius( minimumZoom() ), qMin<int>( radius, d->radius( maximumZoom() ) ) );

        GeoDataLookAt target = lookAt();
        target.setRange( KM2METER * distanceFromRadius( radius ) );

        flyTo( target, mode );
    }
}

void MarbleWidget::zoomOut( FlyToMode mode )
{
    if ( d->m_map.tileZoomLevel() <= 0 ) {
        zoomViewBy( -d->m_zoomStep, mode );
    } else {
        int radius = d->m_map.preferredRadiusFloor( d->m_map.radius() * 0.95 );
        radius = qMax<int>( d->radius( minimumZoom() ), qMin<int>( radius, d->radius( maximumZoom() ) ) );

        GeoDataLookAt target = lookAt();
        target.setRange( KM2METER * distanceFromRadius( radius ) );

        flyTo( target, mode );
    }
}

void MarbleWidget::rotateBy( const qreal deltaLon, const qreal deltaLat, FlyToMode mode )
{
    Quaternion  rotPhi( 1.0, deltaLat / 180.0, 0.0, 0.0 );
    Quaternion  rotTheta( 1.0, 0.0, deltaLon / 180.0, 0.0 );

    Quaternion  axis = d->m_map.viewport()->planetAxis();
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
        bool icon;
        GeoDataCoordinates coords = placemark.coordinate( d->m_model.clock()->dateTime(), &icon );
        if ( icon ) {
            centerOn( coords, animated );
        } else {
            centerOn( placemark.geometry()->latLonAltBox(), animated );
        }
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
    return d->m_map.projection();
}

void MarbleWidget::setProjection( Projection projection )
{
    d->m_map.setProjection( projection );

    update();
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

void MarbleWidget::resizeEvent( QResizeEvent *event )
{
    setUpdatesEnabled( false );
    d->m_map.setSize( event->size() );
    setUpdatesEnabled( true );

    QWidget::resizeEvent( event );
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
                                      qreal& x, qreal& y ) const
{
    return d->m_map.screenCoordinates( lon, lat, x, y );
}

bool MarbleWidget::geoCoordinates( int x, int y,
                                   qreal& lon, qreal& lat,
                                   GeoDataCoordinates::Unit unit ) const
{
    return d->m_map.geoCoordinates( x, y, lon, lat, unit );
}

qreal MarbleWidget::centerLatitude() const
{
    return d->m_map.centerLatitude();
}

qreal MarbleWidget::centerLongitude() const
{
    return d->m_map.centerLongitude();
}

QRegion MarbleWidget::mapRegion() const
{
    return viewport()->mapRegion();
}

void MarbleWidget::paintEvent( QPaintEvent *evt )
{
    QTime t;
    t.start();

    QPaintDevice *paintDevice = this;
    QImage image;
    if (!isEnabled())
    {
        // If the globe covers fully the screen then we can use the faster
        // RGB32 as there are no translucent areas involved.
        QImage::Format imageFormat = ( d->m_map.viewport()->mapCoversViewport() )
                                     ? QImage::Format_RGB32
                                     : QImage::Format_ARGB32_Premultiplied;
        // Paint to an intermediate image
        image = QImage( rect().size(), imageFormat );
        image.fill( Qt::transparent );
        paintDevice = &image;
    }

    {
        // FIXME: Better way to get the GeoPainter
        bool  doClip = true;
        if ( projection() == Spherical )
            doClip = ( radius() > width() / 2
                       || radius() > height() / 2 );

        // Create a painter that will do the painting.
        GeoPainter geoPainter( paintDevice, d->m_map.viewport(),
                               d->m_map.mapQuality(), doClip );

        d->m_map.paint( geoPainter, evt->rect() );
    }

    if ( !isEnabled() )
    {
        // Draw a grayscale version of the intermediate image
        QRgb* pixel = reinterpret_cast<QRgb*>( image.scanLine( 0 ));
        for (int i=0; i<image.width()*image.height(); ++i, ++pixel) {
            int gray = qGray( *pixel );
            *pixel = qRgb( gray, gray, gray );
        }

        QPainter widgetPainter( this );
        widgetPainter.drawImage( rect(), image );
    }

    if ( d->m_showFrameRate )
    {
        QPainter painter( this );
        FpsLayer fpsPainter( &t );
        fpsPainter.paint( &painter );

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
    d->m_model.home( homeLon, homeLat, homeZoom );

    GeoDataLookAt target;
    target.setLongitude( homeLon, GeoDataCoordinates::Degree );
    target.setLatitude( homeLat, GeoDataCoordinates::Degree );
    target.setRange( 1000 * distanceFromZoom( homeZoom ) );

    flyTo( target, mode );
}

QString MarbleWidget::mapThemeId() const
{
    return d->m_model.mapThemeId();
}

void MarbleWidget::setMapThemeId( const QString& mapThemeId )
{
    d->m_map.setMapThemeId( mapThemeId );
}

void MarbleWidgetPrivate::updateMapTheme()
{
    m_map.removeLayer( m_routingLayer );

    m_widget->setRadius( m_widget->radius() ); // Corrects zoom range, if needed

    if ( m_model.planetId() == "earth" ) {
        m_map.addLayer( m_routingLayer );
    }

    emit m_widget->themeChanged( m_map.mapThemeId() );

    // Now we want a full repaint as the atmosphere might differ
    m_widget->setAttribute( Qt::WA_NoSystemBackground, false );

    m_widget->update();
}

GeoSceneDocument *MarbleWidget::mapTheme() const
{
    return d->m_model.mapTheme();
}

void MarbleWidget::setPropertyValue( const QString& name, bool value )
{
    mDebug() << "In MarbleWidget the property " << name << "was set to " << value;
    d->m_map.setPropertyValue( name, value );

    update();
}

void MarbleWidget::setShowOverviewMap( bool visible )
{
    d->m_map.setShowOverviewMap( visible );

    update();
}

void MarbleWidget::setShowScaleBar( bool visible )
{
    d->m_map.setShowScaleBar( visible );

    update();
}

void MarbleWidget::setShowCompass( bool visible )
{
    d->m_map.setShowCompass( visible );

    update();
}

void MarbleWidget::setShowClouds( bool visible )
{
    d->m_map.setShowClouds( visible );

    update();
}

void MarbleWidget::setShowSunShading( bool visible )
{
    d->m_map.setShowSunShading( visible );

    update();
}

void MarbleWidget::setShowCityLights( bool visible )
{
    d->m_map.setShowCityLights( visible );

    update();
}

void MarbleWidget::setLockToSubSolarPoint( bool visible )
{
    if ( d->m_map.isLockedToSubSolarPoint() != visible ) { // Toggling input modifies event filters, so avoid that if not needed
        d->m_map.setLockToSubSolarPoint( visible );
        setInputEnabled( !d->m_map.isLockedToSubSolarPoint() );
    }
}

void MarbleWidget::setSubSolarPointIconVisible( bool visible )
{
    if ( d->m_map.isSubSolarPointIconVisible() != visible ) {
        d->m_map.setSubSolarPointIconVisible( visible );
    }

    QList<RenderPlugin *> pluginList = renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = pluginList.constEnd();
    for (; i != end; ++i ) {
        if ( (*i)->nameId() == "sun" ) {
            (*i)->setVisible( visible );
        }
    }
}

void MarbleWidget::setShowAtmosphere( bool visible )
{
    d->m_map.setShowAtmosphere( visible );

    update();
}

void MarbleWidget::setShowCrosshairs( bool visible )
{
    d->m_map.setShowCrosshairs( visible );

    update();
}

void MarbleWidget::setShowGrid( bool visible )
{
    d->m_map.setShowGrid( visible );

    update();
}

void MarbleWidget::setShowPlaces( bool visible )
{
    d->m_map.setShowPlaces( visible );

    update();
}

void MarbleWidget::setShowCities( bool visible )
{
    d->m_map.setShowCities( visible );

    update();
}

void MarbleWidget::setShowTerrain( bool visible )
{
    d->m_map.setShowTerrain( visible );

    update();
}

void MarbleWidget::setShowOtherPlaces( bool visible )
{
    d->m_map.setShowOtherPlaces( visible );

    update();
}

void MarbleWidget::setShowRelief( bool visible )
{
    d->m_map.setShowRelief( visible );

    update();
}

void MarbleWidget::setShowIceLayer( bool visible )
{
    d->m_map.setShowIceLayer( visible );

    update();
}

void MarbleWidget::setShowBorders( bool visible )
{
    d->m_map.setShowBorders( visible );

    update();
}

void MarbleWidget::setShowRivers( bool visible )
{
    d->m_map.setShowRivers( visible );

    update();
}

void MarbleWidget::setShowLakes( bool visible )
{
    d->m_map.setShowLakes( visible );

    update();
}

void MarbleWidget::setShowFrameRate( bool visible )
{
    d->m_showFrameRate = visible;

    update();
}

void MarbleWidget::setShowBackground( bool visible )
{
    d->m_map.setShowBackground( visible );

    update();
}

void MarbleWidget::setShowRuntimeTrace( bool visible )
{
    d->m_map.setShowRuntimeTrace( visible );
}

void MarbleWidget::setShowTileId( bool visible )
{
    d->m_map.setShowTileId( visible );
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
    d->m_map.clearVolatileTileCache();
}

void MarbleWidget::setVolatileTileCacheLimit( quint64 kiloBytes )
{
    d->m_map.setVolatileTileCacheLimit( kiloBytes );
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

MapQuality MarbleWidget::mapQuality( ViewContext viewContext ) const
{
    return d->m_map.mapQuality( viewContext );
}

void MarbleWidget::setMapQualityForViewContext( MapQuality quality, ViewContext viewContext )
{
    const MapQuality oldQuality = d->m_map.mapQuality();

    d->m_map.setMapQualityForViewContext( quality, viewContext );

    if ( d->m_map.mapQuality() != oldQuality )
        update();
}

ViewContext MarbleWidget::viewContext() const
{
    return d->m_map.viewContext();
}

void MarbleWidget::setViewContext( ViewContext viewContext )
{
    if ( d->m_map.viewContext() != viewContext ) {
        const MapQuality oldQuality = d->m_map.mapQuality();
        d->m_map.setViewContext( viewContext );
        d->m_routingLayer->setViewContext( viewContext );

        if ( d->m_map.mapQuality() != oldQuality )
            update();
    }
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
    return d->m_map.defaultAngleUnit();
}

void MarbleWidget::setDefaultAngleUnit( AngleUnit angleUnit )
{
    d->m_map.setDefaultAngleUnit( angleUnit );
}

QFont MarbleWidget::defaultFont() const
{
    return d->m_map.defaultFont();
}

void MarbleWidget::setDefaultFont( const QFont& font )
{
    d->m_map.setDefaultFont( font );
}

void MarbleWidget::setSelection( const QRect& region )
{
    QPoint tl = region.topLeft();
    QPoint br = region.bottomRight();
    mDebug() << "Selection region: (" << tl.x() << ", " <<  tl.y() << ") (" 
             << br.x() << ", " << br.y() << ")" << endl;

    GeoDataLatLonAltBox box  = viewport()->latLonAltBox( region );

    // NOTE: coordinates as lon1, lat1, lon2, lat2 (or West, North, East, South)
    // as left/top, right/bottom rectangle.
    QList<double> coordinates;
    coordinates << box.west( GeoDataCoordinates::Degree ) << box.north( GeoDataCoordinates::Degree )
                << box.east( GeoDataCoordinates::Degree ) << box.south( GeoDataCoordinates::Degree );

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

    const QLocale::MeasurementSystem measurementSystem = MarbleGlobal::getInstance()->locale()->measurementSystem();

    switch ( measurementSystem ) {
    case QLocale::MetricSystem:
        distanceUnitString = tr("km");
        break;
    case QLocale::ImperialSystem:
        dist *= KM2MI;
        distanceUnitString = tr("mi");
        break;
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
    return d->m_map.renderPlugins();
}

void MarbleWidget::readPluginSettings( QSettings& settings )
{
    disconnect( &d->m_map, SIGNAL( pluginSettingsChanged() ),
                this,      SIGNAL( pluginSettingsChanged() ) );

    foreach( RenderPlugin *plugin, renderPlugins() ) {
        settings.beginGroup( QString( "plugin_" ) + plugin->nameId() );

        QHash<QString,QVariant> hash;

        foreach ( const QString& key, settings.childKeys() ) {
            hash.insert( key, settings.value( key ) );
        }

        plugin->setSettings( hash );

        settings.endGroup();
    }

    connect( &d->m_map, SIGNAL( pluginSettingsChanged() ),
             this,      SIGNAL( pluginSettingsChanged() ) );
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
    return d->m_map.floatItems();
}

AbstractFloatItem * MarbleWidget::floatItem( const QString &nameId ) const
{
    return d->m_map.floatItem( nameId );
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
            d->m_map.setRadius( radius );
            d->m_logzoom = qRound( d->zoom( radius ) );

            GeoDataCoordinates::Unit deg = GeoDataCoordinates::Degree;
            d->m_map.centerOn( newLookAt.longitude( deg ), newLookAt.latitude( deg ) );

            emit zoomChanged( d->m_logzoom );
            emit distanceChanged( distanceString() );

            update();
        }
    }
    else {
        d->m_physics.flyTo( newLookAt, mode );
    }
}

void MarbleWidget::reloadMap()
{
    d->m_map.reload();
}

void MarbleWidget::downloadRegion( QVector<TileCoordsPyramid> const & pyramid )
{
    d->m_map.downloadRegion( pyramid );
}

GeoDataLookAt MarbleWidget::lookAt() const
{
    GeoDataLookAt result;

    result.setLongitude( d->m_map.viewport()->centerLongitude() );
    result.setLatitude( d->m_map.viewport()->centerLatitude() );
    result.setAltitude( 0.0 );
    result.setRange( distance() * KM2METER );

    return result;
}

GeoDataCoordinates MarbleWidget::focusPoint() const
{
    return d->m_map.viewport()->focusPoint();
}

void MarbleWidget::setFocusPoint( const GeoDataCoordinates &focusPoint )
{
    d->m_map.viewport()->setFocusPoint( focusPoint );
}

void MarbleWidget::resetFocusPoint()
{
    d->m_map.viewport()->resetFocusPoint();
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

MapInfoDialog *MarbleWidget::mapInfoDialog()
{
    return d->m_mapInfoDialog;
}

}

#include "MarbleWidget.moc"
