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
#include "MergedLayerDecorator.h"
#include "RenderPlugin.h"
#include "SunLocator.h"
#include "TileCreatorDialog.h"
#include "ViewportParams.h"
#include "BookmarkManager.h"
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
          m_inputhandler( 0 ),
          m_physics( new MarblePhysics( parent ) ),
          m_repaintTimer()
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
    void moveByStep( MarbleWidget* widget, int stepsRight, int stepsDown, FlyToMode mode );

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

    // Some values from m_map, as they were last time we repainted.
    // To store them here will save some repaintings.
    int              m_logZoom;

    MarbleWidgetInputHandler  *m_inputhandler;

    MarblePhysics    *m_physics;

    // For scheduling repaints
    QTimer           m_repaintTimer;
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
    m_widget->connect( m_model,  SIGNAL( modelChanged() ),
		       m_widget, SLOT( updateChangedMap() ) );

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

    m_logZoom  = 0;

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

    m_widget->connect( m_model, SIGNAL( pluginSettingsChanged() ),
                       m_widget, SIGNAL( pluginSettingsChanged() ) );
                       
    m_widget->connect( m_model, SIGNAL( renderPluginInitialized( RenderPlugin * ) ),
                       m_widget, SIGNAL( renderPluginInitialized( RenderPlugin * ) ) );
}

void MarbleWidgetPrivate::moveByStep( MarbleWidget* widget, int stepsRight, int stepsDown, FlyToMode mode )
{
    int polarity = m_map->viewport()->polarity();
    qreal left = polarity * stepsRight * m_map->moveStep();
    qreal down = stepsDown * m_map->moveStep();
    widget->rotateBy( left, down, mode );
}

void MarbleWidgetPrivate::repaint()
{
    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    m_widget->setAttribute( Qt::WA_NoSystemBackground,
                  m_map->mapCoversViewport() && !m_model->mapThemeId().isEmpty() );

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
    return d->m_map->planetAxis();
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


void MarbleWidget::zoomView( int newZoom, FlyToMode mode )
{
    if ( mode == Instant || !d->m_animationsEnabled ) {
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
        d->repaint();
    }
    else {
        GeoDataLookAt target = d->m_map->lookAt();
        int radius = d->m_map->d->radius( newZoom );
        target.setRange( 1000 * d->m_map->distanceFromRadius( radius ) );
        flyTo( target, mode == Automatic ? Instant : mode );
    }
}


void MarbleWidget::zoomViewBy( int zoomStep, FlyToMode mode )
{
    zoomView( zoom() + zoomStep, mode );
}


void MarbleWidget::zoomIn( FlyToMode mode )
{
    if ( mode == Instant || !d->m_animationsEnabled ) {
        d->m_map->zoomIn();
        d->repaint();
    }
    else {
        GeoDataLookAt target = d->m_map->lookAt();
        MarbleMap *map = d->m_map;
        int newRadius = map->d->radius( map->zoom() + map->d->m_zoomStep );
        target.setRange( 1000 * d->m_map->distanceFromRadius( newRadius ) );
        flyTo( target, mode );
    }
}

void MarbleWidget::zoomOut( FlyToMode mode )
{
    if ( mode == Instant || !d->m_animationsEnabled ) {
        d->m_map->zoomOut();
        d->repaint();
    }
    else {
        GeoDataLookAt target = d->m_map->lookAt();
        MarbleMap *map = d->m_map;
        int newRadius = map->d->radius( map->zoom() - map->d->m_zoomStep );
        target.setRange( 1000 * d->m_map->distanceFromRadius( newRadius ) );
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

    Quaternion  axis = d->m_map->planetAxis();
    qreal lon( 0.0 ), lat( 0.0 );
    axis.getSpherical( lon, lat );
    axis = rotTheta * axis;
    axis *= rotPhi;
    axis.normalize();
    lat = -axis.pitch();
    lon = axis.yaw();
    
    GeoDataLookAt target = d->m_map->lookAt();
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
    QItemSelectionModel *selectionModel = d->m_map->model()->placemarkSelectionModel();
    Q_ASSERT( selectionModel );

    selectionModel->clear();

    if ( index.isValid() ) {
        const GeoDataCoordinates targetPosition =
            index.data( MarblePlacemarkModel::CoordinateRole ).value<GeoDataCoordinates>();

        GeoDataLookAt target = d->m_map->lookAt();
        target.setLongitude( targetPosition.longitude() );
        target.setLatitude( targetPosition.latitude() );
        flyTo( target, animated ? Automatic : Instant );

        selectionModel->select( index, QItemSelectionModel::SelectCurrent );
    }
}

void MarbleWidget::centerOn( const GeoDataCoordinates &position, bool animated )
{
    GeoDataLookAt target = d->m_map->lookAt();
    target.setLongitude( position.longitude() );
    target.setLatitude( position.latitude() );
    flyTo( target, animated ? Automatic : Instant );
}

void MarbleWidget::centerOn( const GeoDataLatLonBox &box, bool animated )
{
    Q_UNUSED( animated );

    ViewportParams* viewparams = d->m_map->viewport();
    //prevent divide by zero
    if( box.height() && box.width() ) {
        //work out the needed zoom level
        int horizontalRadius = ( 0.25 * M_PI ) * ( viewparams->height() / box.height() );
        int verticalRadius = ( 0.25 * M_PI ) * ( viewparams->width() / box.width() );
        setRadius( qMin<int>( horizontalRadius, verticalRadius ) );
    }

    //move the map
    d->m_map->centerOn( box.center().longitude( GeoDataCoordinates::Degree ),
                        box.center().latitude( GeoDataCoordinates::Degree ) );

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
    d->m_map->home( lon, lat, zoom );
}

void MarbleWidget::setHome( qreal lon, qreal lat, int zoom )
{
    d->m_map->setHome( lon, lat, zoom );
}

void MarbleWidget::setHome( const GeoDataCoordinates& homePoint, int zoom )
{
    d->m_map->setHome( homePoint, zoom );
}


void MarbleWidget::moveLeft( FlyToMode mode )
{
    d->moveByStep( this, -1, 0, mode );
}

void MarbleWidget::moveRight( FlyToMode mode )
{
    d->moveByStep( this, 1, 0, mode );
}


void MarbleWidget::moveUp( FlyToMode mode )
{
    d->moveByStep( this, 0, -1, mode );
}

void MarbleWidget::moveDown( FlyToMode mode )
{
    d->moveByStep( this, 0, 1, mode );
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

int MarbleWidget::northPoleY()
{
    return northPolePosition().y();
}

QPoint MarbleWidget::northPolePosition()
{
    return d->m_map->northPolePosition();
}

QPoint MarbleWidget::southPolePosition()
{
    return d->m_map->southPolePosition();
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
    return d->m_map->viewport()->activeRegion();
}

QRegion MarbleWidget::mapRegion()
{
    return d->m_map->viewport()->currentProjection()->mapRegion( d->m_map->viewport() );
}

void MarbleWidget::paintEvent( QPaintEvent *evt )
{
    // Stop repaint timer if it is already running
    d->m_repaintTimer.stop();
    QTime t;
    t.start();

    // FIXME: Better way to get the GeoPainter
    bool  doClip = true;
    if ( d->m_map->projection() == Spherical )
        doClip = ( d->m_map->radius() > width() / 2
                   || d->m_map->radius() > height() / 2 );

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
    d->m_map->d->paintOverlay( painter, dirtyRect );

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

    if ( d->m_map->showFrameRate() )
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
    if ( !d->m_animationsEnabled || mode == Instant )
    {
        d->m_map->goHome();

         // not obsolete in case the zoomlevel stays unaltered
        d->repaint();
    }
    else {
        qreal  homeLon = 0;
        qreal  homeLat = 0;
        int homeZoom = 0;
        d->m_map->home( homeLon, homeLat, homeZoom );

        GeoDataLookAt target;
        target.setLongitude( homeLon, GeoDataCoordinates::Degree );
        target.setLatitude( homeLat, GeoDataCoordinates::Degree );
        int radius = d->m_map->d->radius( homeZoom );
        target.setRange( 1000 * d->m_map->distanceFromRadius( radius ) );

        flyTo( target, mode );
    }
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
    
    // Update texture map during the repaint that follows:
    setNeedsUpdate();

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
    d->m_map->openGpxFile( filename );
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

void MarbleWidget::updateChangedMap()
{
    // Update texture map during the repaint that follows:
    setNeedsUpdate();
    update();
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
        map()->setMapQuality( d->m_stillQuality ); 
    }
    else if ( viewContext() == Animation )
    {
        map()->setMapQuality( d->m_animationQuality ); 
    }
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

    AbstractProjection *proj = d->m_map->viewport()->currentProjection();
    GeoDataLatLonAltBox box  = proj->latLonAltBox( region, d->m_map->viewport() );

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
    // Update the sun shading.
    //SunLocator  *sunLocator = d->m_model->sunLocator();

    mDebug() << "MarbleWidget: Updating the sun shading map...";
    d->m_model->update();
    setNeedsUpdate();
    //mDebug() << "Finished updating the sun shading map";
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
    return d->m_model->renderPlugins();
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
    return d->m_model->floatItems();
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

void MarbleWidget::flyTo( const GeoDataLookAt &lookAt, FlyToMode mode )
{
    if ( !d->m_animationsEnabled || mode == Instant ) {
        d->m_map->flyTo( lookAt );
        d->repaint();
    }
    else {
        GeoDataLookAt source = d->m_map->lookAt();
        setViewContext( Marble::Animation );
        ViewportParams *viewport = d->m_map->viewport();
        d->m_physics->flyTo( source, lookAt, viewport, mode );
    }
}

void MarbleWidget::reloadMap()
{
    d->m_model->reloadMap();
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
    setNeedsUpdate();
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

QVector<GeoDataFolder*> MarbleWidget::folders()
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
             
}

#include "MarbleWidget.moc"
