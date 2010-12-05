//
// This file is part of the Marble Virtual Globe.
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
#include "FileViewModel.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoPainter.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleMap.h"
#include "MarbleMap_p.h" // FIXME: remove this
#include "MarbleModel.h"
#include "MarblePhysics.h"
#include "MarblePlacemarkModel.h"
#include "MarbleWidgetInputHandler.h"
#include "MeasureTool.h"
#include "MergedLayerDecorator.h"
#include "RenderPlugin.h"
#include "SunLocator.h"
#include "TileCreatorDialog.h"
#include "ViewportParams.h"
#include "BookmarkManager.h"
#include "routing/RoutingLayer.h"
#include "routing/RoutingManager.h"
#include "routing/AlternativeRoutesModel.h"

namespace Marble
{

#ifdef Q_CC_MSVC
# ifndef KDEWIN_MATH_H
   static long double sqrt( int a ) { return sqrt( (long double)a ); }
# endif
#endif

const int REPAINT_SCHEDULING_INTERVAL = 1000;


class MarbleWidgetPrivate
{
 public:
    MarbleWidgetPrivate( MarbleMap *map, MarbleWidget *parent )
        : m_widget( parent ),
          m_map( map ),
          m_model( map->model() ),
          m_viewContext( Still ),
          m_stillQuality( HighQuality ),
          m_animationQuality( LowQuality ),
          m_animationsEnabled( false ),
          m_zoomStep( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ? 60 : 40 ),
          m_inputhandler( 0 ),
          m_physics( new MarblePhysics( parent ) ),
          m_repaintTimer(),
          m_routingLayer( 0 )
    {
    }

    ~MarbleWidgetPrivate()
    {
        delete m_map;
    }

    void  construct();

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

    ViewContext     m_viewContext;

    MapQuality      m_stillQuality;
    MapQuality      m_animationQuality;

    bool m_animationsEnabled;

    int              m_zoomStep;

    MarbleWidgetInputHandler  *m_inputhandler;

    MarblePhysics    *m_physics;

    // For scheduling repaints
    QTimer           m_repaintTimer;

    RoutingLayer     *m_routingLayer;
};



MarbleWidget::MarbleWidget(QWidget *parent)
    : QWidget( parent ),
      d( new MarbleWidgetPrivate( new MarbleMap(), this ) )
{
//    setAttribute( Qt::WA_PaintOnScreen, true );
    d->construct();
}


MarbleWidget::MarbleWidget(MarbleMap *map, QWidget *parent)
    : QWidget( parent ),
      d( new MarbleWidgetPrivate( map, this ) )
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
    m_widget->setAttribute( Qt::WA_AcceptTouchEvents );
#endif

    // Initialize the map and forward some signals.
    m_map->setSize( m_widget->width(), m_widget->height() );
    m_map->setMapQuality( m_stillQuality );

    m_widget->connect( m_map,    SIGNAL( projectionChanged( Projection ) ),
                       m_widget, SIGNAL( projectionChanged( Projection ) ) );

    // When some fundamental things change in the model, we got to
    // show this in the view, i.e. here.
    m_widget->connect( m_model,  SIGNAL( themeChanged( QString ) ),
		       m_widget, SIGNAL( themeChanged( QString ) ) );
    m_widget->connect( m_model, SIGNAL( modelChanged() ),
                       m_widget, SLOT( update() ) );

    // Repaint scheduling
    m_widget->connect( m_map,    SIGNAL( repaintNeeded( QRegion ) ),
                       m_widget, SLOT( scheduleRepaint( QRegion ) ) );
    m_repaintTimer.setSingleShot( true );
    m_repaintTimer.setInterval( REPAINT_SCHEDULING_INTERVAL );
    m_widget->connect( &m_repaintTimer, SIGNAL( timeout() ),
                       m_widget, SLOT( update() ) );

    // When some fundamental things change in the map, we got to show
    // this in the view, i.e. here.
    m_widget->connect( m_map,    SIGNAL( zoomChanged( int ) ),
                       m_widget, SIGNAL( zoomChanged( int ) ) );
    m_widget->connect( m_map,    SIGNAL( distanceChanged( QString ) ),
                       m_widget, SIGNAL( distanceChanged( QString ) ) );
    m_widget->connect( m_map,    SIGNAL( visibleLatLonAltBoxChanged( GeoDataLatLonAltBox )),
                       m_widget, SIGNAL( visibleLatLonAltBoxChanged( GeoDataLatLonAltBox )));
    m_widget->connect( m_map,    SIGNAL( tileLevelChanged( int ) ),
                       m_widget, SIGNAL( tileLevelChanged( int ) ) );

    m_widget->connect( m_map,    SIGNAL( pluginSettingsChanged() ),
                       m_widget, SIGNAL( pluginSettingsChanged() ) );
    m_widget->connect( m_map,    SIGNAL( renderPluginInitialized( RenderPlugin * ) ),
                       m_widget, SIGNAL( renderPluginInitialized( RenderPlugin * ) ) );

    // Set background: black.
    m_widget->setPalette( QPalette ( Qt::black ) );

    // Set whether the black space gets displayed or the earth gets simply 
    // displayed on the widget background.
    m_widget->setAutoFillBackground( true );

    // Show a progress dialog when the model calculates new map tiles.
    m_widget->connect( m_model, SIGNAL( creatingTilesStart( TileCreator*, const QString&,
                                                            const QString& ) ),
                       m_widget, SLOT( creatingTilesStart( TileCreator*, const QString&,
                                                           const QString& ) ) );

    m_widget->connect( m_model->sunLocator(), SIGNAL( enableWidgetInput( bool ) ),
                       m_widget, SLOT( setInputEnabled( bool ) ) );

    m_widget->connect( m_model->sunLocator(), SIGNAL( updateStars() ),
                       m_widget, SLOT( update() ) );

    m_widget->connect( m_physics, SIGNAL( positionReached( GeoDataLookAt ) ),
                       m_widget, SLOT( updateAnimation( GeoDataLookAt ) ) );

    m_widget->connect( m_physics, SIGNAL( finished() ),
                       m_widget, SLOT( startStillMode() ) );

    m_widget->connect( m_model->sunLocator(), SIGNAL( centerSun() ),
                       m_widget, SLOT( centerSun() ) );

    m_widget->setInputHandler( new MarbleWidgetDefaultInputHandler );
    m_widget->setMouseTracking( m_widget );

    m_routingLayer = new RoutingLayer( m_widget, m_widget );
    m_routingLayer->setRouteRequest( m_model->routingManager()->routeRequest() );
    m_routingLayer->setModel( m_model->routingManager()->routingModel() );
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


MarbleMap *MarbleWidget::map() const
{
    return d->m_map;
}

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


void MarbleWidget::setInputHandler( MarbleWidgetInputHandler *handler )
{
    delete d->m_inputhandler;
    d->m_inputhandler = handler;

    if ( d->m_inputhandler ) {
        d->m_inputhandler->init( this );
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
    if ( radius == d->m_map->radius() ) {
        return;
    }
        
    d->m_map->setRadius( radius );
    d->repaint();
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
    if ( radius() < sqrt( (qreal)(width() * width() + height() * height()) ) )
        return 180.0 * 0.1;
    else
        return 180.0 * atan( (qreal)width()
                     / (qreal)( 2 * radius() ) ) * 0.2;
}

int MarbleWidget::zoom() const
{
    return d->m_map->zoom();
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

void MarbleWidget::addPlacemarkFile( const QString &filename )
{
    addGeoDataFile( filename );
}

void MarbleWidget::addPlacemarkData( const QString &data, const QString &key )
{
    addGeoDataString( data, key );
}

void MarbleWidget::removePlacemarkKey( const QString &key )
{
    removeGeoData( key );
}

QVector<QModelIndex> MarbleWidget::whichFeatureAt( const QPoint &curpos ) const
{
    return d->m_map->whichFeatureAt( curpos );
}

QList<AbstractDataPluginItem*> MarbleWidget::whichItemAt( const QPoint &curpos ) const
{
    return d->m_map->whichItemAt( curpos );
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


void MarbleWidget::zoomView( int newZoom, FlyToMode mode )
{
    // It won't fly anyway. So we should do everything to keep the zoom value.
    if ( !d->m_animationsEnabled || mode == Instant ) {
        d->m_map->zoomView( newZoom );
        d->repaint();
    }
    else {
        GeoDataLookAt target = lookAt();
        target.setRange( KM2METER * d->m_map->distanceFromZoom( newZoom ) );

        flyTo( target, mode );
    }
}


void MarbleWidget::zoomViewBy( int zoomStep, FlyToMode mode )
{
    zoomView( zoom() + zoomStep, mode );
}


void MarbleWidget::zoomIn( FlyToMode mode )
{
    zoomViewBy( d->m_zoomStep, mode );
}

void MarbleWidget::zoomOut( FlyToMode mode )
{
    zoomViewBy( -d->m_zoomStep, mode );
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

void MarbleWidget::centerOn( const QModelIndex& index, bool animated )
{
    QItemSelectionModel *selectionModel = d->m_model->placemarkSelectionModel();
    Q_ASSERT( selectionModel );

    selectionModel->clear();

    if ( index.isValid() ) {
        const GeoDataCoordinates targetPosition =
            index.data( MarblePlacemarkModel::CoordinateRole ).value<GeoDataCoordinates>();

        GeoDataLookAt target = lookAt();
        target.setLongitude( targetPosition.longitude() );
        target.setLatitude( targetPosition.latitude() );
        flyTo( target, animated ? Automatic : Instant );

        selectionModel->select( index, QItemSelectionModel::SelectCurrent );
    }
}

void MarbleWidget::centerOn( const GeoDataCoordinates &position, bool animated )
{
    GeoDataLookAt target = lookAt();
    target.setLongitude( position.longitude() );
    target.setLatitude( position.latitude() );
    flyTo( target, animated ? Automatic : Instant );
}

void MarbleWidget::centerOn( const GeoDataLatLonBox &box, bool animated )
{
    ViewportParams* viewparams = viewport();
    //prevent divide by zero
    if( box.height() && box.width() ) {
        //work out the needed zoom level
        int horizontalRadius = ( 0.25 * M_PI ) * ( viewparams->height() / box.height() );
        int verticalRadius = ( 0.25 * M_PI ) * ( viewparams->width() / box.width() );
        setRadius( qMin<int>( horizontalRadius, verticalRadius ) );
    }

    //move the map
    centerOn( box.center().longitude( GeoDataCoordinates::Degree ),
              box.center().latitude( GeoDataCoordinates::Degree ),
              animated );

    repaint();
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

void MarbleWidget::home( qreal &lon, qreal &lat, int& zoom )
{
    d->m_model->home( lon, lat, zoom );
}

void MarbleWidget::setHome( qreal lon, qreal lat, int zoom )
{
    d->m_model->setHome( lon, lat, zoom );
}

void MarbleWidget::setHome( const GeoDataCoordinates& homePoint, int zoom )
{
    d->m_model->setHome( homePoint, zoom );
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
        QImage::Format imageFormat = ( map()->mapCoversViewport() )
                                     ? QImage::Format_RGB32
                                     : QImage::Format_ARGB32_Premultiplied;
        // Paint to an intermediate image
        image = QImage( rect().size(), imageFormat );
        image.fill( Qt::transparent );
        paintDevice = &image;
    }

    // Create a painter that will do the painting.
    GeoPainter painter( paintDevice, map()->viewport(),
                        map()->mapQuality(), doClip );
    QRect  dirtyRect = evt->rect();

    // Draws the map like MarbleMap::paint does, but adds our customPaint in between
    d->m_map->d->paintGround( painter, dirtyRect );
    d->m_map->customPaint( &painter );
    customPaint( &painter );
    d->m_map->measureTool()->render( &painter, viewport() );

    if ( !isEnabled() )
    {
        // Draw a grayscale version of the intermediate image
        QRgb* pixel = reinterpret_cast<QRgb*>( image.scanLine( 0 ));
        for (int i=0; i<image.width()*image.height(); ++i, ++pixel) {
            int gray = qGray( *pixel );
            *pixel = qRgb( gray, gray, gray );
        }

        GeoPainter widgetPainter( this, map()->viewport(),
                            map()->mapQuality(), doClip );
        widgetPainter.drawImage( rect(), image );
    }

    if ( showFrameRate() )
    {
        qreal fps = 1000.0 / (qreal)( t.elapsed() + 1 );
        d->m_map->d->paintFps( painter, dirtyRect, fps );
        emit d->m_map->framesPerSecond( fps );
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
    target.setRange( 1000 * d->m_map->distanceFromZoom( homeZoom ) );

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

    centerSun();

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

void MarbleWidget::openGpxFile( const QString &filename )
{
    addGeoDataFile( filename );
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
    if ( viewContext == Still )
        return d->m_stillQuality;

    Q_ASSERT( viewContext == Animation );
    return d->m_animationQuality; 
}

void MarbleWidget::setMapQuality( MapQuality quality, ViewContext changedViewContext )
{
    const MapQuality oldQuality = mapQuality( viewContext() );

    // FIXME: Rewrite as a switch
    if ( changedViewContext == Still ) {
        d->m_stillQuality = quality;
    }
    else if ( changedViewContext == Animation ) {
        d->m_animationQuality = quality;
    }

    if ( viewContext() == Still ) {
        map()->setMapQuality( d->m_stillQuality ); 
    }
    else if ( viewContext() == Animation )
    {
        map()->setMapQuality( d->m_animationQuality ); 
    }

    if ( mapQuality( viewContext() ) != oldQuality )
        d->repaint();
}

ViewContext MarbleWidget::viewContext() const
{
    return d->m_viewContext;
}

void MarbleWidget::setViewContext( ViewContext viewContext )
{
    d->m_viewContext = viewContext;

    if ( viewContext == Still )
        map()->setMapQuality( d->m_stillQuality ); 
    if ( viewContext == Animation )
        map()->setMapQuality( d->m_animationQuality ); 

    if ( mapQuality( viewContext ) != mapQuality( Animation ) )
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
    return map()->defaultAngleUnit();
}

void MarbleWidget::setDefaultAngleUnit( AngleUnit angleUnit )
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

void MarbleWidget::setSelection( const QRect& region )
{
    QPoint tl = region.topLeft();
    QPoint br = region.bottomRight();
    mDebug() << "Selection region: (" << tl.x() << ", " <<  tl.y() << ") (" 
             << br.x() << ", " << br.y() << ")" << endl;

    AbstractProjection *proj = viewport()->currentProjection();
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
    d->m_map->updateSun();
}

void MarbleWidget::centerSun()
{
    SunLocator  *sunLocator = d->m_model->sunLocator();

    if ( sunLocator && sunLocator->getCentered() ) {
        qreal  lon = sunLocator->getLon();
        qreal  lat = sunLocator->getLat();
        centerOn( lon, lat );

        setInputEnabled( false );
    }
}

SunLocator* MarbleWidget::sunLocator()
{
    return d->m_model->sunLocator();
}

void MarbleWidget::setInputEnabled( bool enabled )
{
    //if input is set as enabled
    if ( enabled )
    {
        if ( !d->m_inputhandler ) {
            setInputHandler( new MarbleWidgetDefaultInputHandler );
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
        d->m_map->flyTo( newLookAt );
        d->repaint();
    }
    else {
        setViewContext( Marble::Animation );
        d->m_physics->flyTo( lookAt(), newLookAt, viewport(), mode );
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

void MarbleWidget::updateAnimation( const GeoDataLookAt &lookAt )
{
    setViewContext( Marble::Animation );
    d->m_map->flyTo( lookAt );
    d->repaint();
}

void MarbleWidget::startStillMode()
{
    setViewContext( Marble::Still );
    d->repaint();
}

GeoDataLookAt MarbleWidget::lookAt() const
{
    return d->m_map->lookAt();
}

void MarbleWidget::addBookmark( const GeoDataPlacemark &bookmark, const QString &folderName ) const
{
    d->m_model->bookmarkManager()->addBookmark( bookmark, folderName );
}

QString MarbleWidget::bookmarkFile() const
{
    return d->m_model->bookmarkManager()->bookmarkFile();
}

bool MarbleWidget::loadBookmarkFile( const QString &relativeFileName )
{
    return d->m_model->bookmarkManager()->loadFile( relativeFileName );
}

QVector<GeoDataFolder*> MarbleWidget::bookmarkFolders()
{
    return d->m_model->bookmarkManager()->folders();
}

void MarbleWidget::removeAllBookmarks()
{
    d->m_model->bookmarkManager()->removeAllBookmarks();
}

void MarbleWidget::addNewBookmarkFolder( const QString &name ) const
{
    d->m_model->bookmarkManager()->addNewBookmarkFolder( name );
}

RoutingLayer* MarbleWidget::routingLayer()
{
    return d->m_routingLayer;
}

void MarbleWidget::addGeoDataFile( const QString &filename )
{
    d->m_map->addGeoDataFile( filename );
    //d->m_model->addGeoDataFile( filename );
}

void MarbleWidget::addGeoDataString( const QString &data, const QString &key )
{
    d->m_map->addGeoDataString( data, key );
}

void MarbleWidget::removeGeoData( const QString &key )
{
    d->m_map->removeGeoData( key );
}

}

#include "MarbleWidget.moc"
