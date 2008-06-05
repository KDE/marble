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

#include "MarbleWidget.h"

#include <cmath>

#include <QtCore/QCoreApplication>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QItemSelectionModel>
#include <QtGui/QSizePolicy>
#include <QtGui/QRegion>
#include <QtGui/QStyleOptionGraphicsItem>
#include <QtNetwork/QNetworkProxy>

//#include <QtDBus/QDBusConnection>

#include "AbstractProjection.h"
#include "MarbleMap.h"
#include "MarbleModel.h"
#include "Quaternion.h"
#include "ViewParams.h"
#include "GeoPainter.h"
#include "FileViewModel.h"
#include "GeoDataPoint.h"
#include "GpxFileViewItem.h"
#include "MarbleDirs.h"
#include "MarblePhysics.h"
#include "MarblePlacemarkModel.h"
#include "MarbleWidgetInputHandler.h"
#include "MarbleWidgetPopupMenu.h"
#include "TileCreatorDialog.h"
#include "HttpDownloadManager.h"
#include "gps/GpsLayer.h"
#include "BoundingBox.h"
#include "SunLocator.h"
#include "MergedLayerDecorator.h"
#include "AbstractProjectionHelper.h"
#include "ViewportParams.h"

#include "MeasureTool.h"

#include "MarbleMap_p.h"


#ifdef Q_CC_MSVC
# ifndef KDEWIN_MATH_H
   static long double sqrt(int a) { return sqrt((long double)a); }
# endif
#endif


// FIXME(IW): Why are the construct and setActiveRegion functions in
//            MarbleWidgetPrivate??  That class is only supposed to
//            hold data for binary compatibility, not have functions.


class MarbleWidgetPrivate
{
 public:
    MarbleWidgetPrivate( MarbleMap *map, MarbleWidget *parent )
        : m_widget( parent ),
          m_map( map ), 
          m_viewContext( Marble::Still ),
          m_stillQuality( Marble::High ), m_animationQuality( Marble::Low ),
          m_inputhandler( 0 ),
          m_physics( new MarblePhysics() ),
          m_proxyHost(),
          m_proxyPort( 0 )
    {
        m_model = m_map->model();
    }

    ~MarbleWidgetPrivate()
    {
        delete m_physics;
        delete m_map;
    }

    void  construct();
    void  setActiveRegion();

    MarbleWidget    *m_widget;
    // The model we are showing.
    MarbleMap       *m_map;
    MarbleModel     *m_model;   // Owned by m_map.  Don't delete.

    ViewContext     m_viewContext;

    MapQuality      m_stillQuality;
    MapQuality      m_animationQuality;

    // Some values from m_map, as they were last time we repainted.
    // To store them here will save some repaintings.
    int              m_logZoom;

    MarbleWidgetInputHandler  *m_inputhandler;
    MarbleWidgetPopupMenu     *m_popupmenu;

    MarblePhysics    *m_physics;

    QString          m_proxyHost;
    qint16           m_proxyPort;
};



MarbleWidget::MarbleWidget(QWidget *parent)
    : QWidget( parent ),
      d( new MarbleWidgetPrivate( new MarbleMap(), this ) )
{
//    QDBusConnection::sessionBus().registerObject("/marble", this, QDBusConnection::QDBusConnection::ExportAllSlots);

    d->construct();
}


MarbleWidget::MarbleWidget(MarbleMap *map, QWidget *parent)
    : QWidget( parent ),
      d( new MarbleWidgetPrivate( map, this ) )
{
//    QDBusConnection::sessionBus().registerObject("/marble", this, QDBusConnection::QDBusConnection::ExportAllSlots);

    d->construct();
}

MarbleWidget::~MarbleWidget()
{
    // Remove and delete an existing InputHandler
    setInputHandler( 0 );
    setDownloadManager( 0 );

    delete d;
}

void MarbleWidgetPrivate::construct()
{
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


    // Some part of the screen contents changed.
    m_widget->connect( m_model, SIGNAL( regionChanged( BoundingBox& ) ) ,
                       m_widget, SLOT( updateRegion( BoundingBox& ) ) );


    // Set background: black.
    m_widget->setPalette( QPalette ( Qt::black ) );

    // Set whether the black space gets displayed or the earth gets simply 
    // displayed on the widget background.
    m_widget->setAutoFillBackground( true );

    m_inputhandler = 0;
    m_popupmenu    = new MarbleWidgetPopupMenu( m_widget, m_model );

    // Handle mouse and keyboard input.
    m_widget->setInputHandler( new MarbleWidgetDefaultInputHandler );
    m_widget->setMouseTracking( true );

    // The interface to the measure tool consists of a RMB popup menu
    // and some signals.
    MeasureTool  *measureTool = m_map->measureTool();
    m_widget->connect( m_popupmenu, SIGNAL( addMeasurePoint( double, double ) ),
                       measureTool, SLOT( addMeasurePoint( double, double ) ) );
    m_widget->connect( m_popupmenu, SIGNAL( removeMeasurePoints() ),
                       measureTool, SLOT( removeMeasurePoints( ) ) );

    // Track the GPS current point at timely intervals.
    m_widget->connect( m_model, SIGNAL( timeout() ),
                       m_widget, SLOT( updateGps() ) );

    // Show a progress dialog when the model calculates new map tiles.
    m_widget->connect( m_model, SIGNAL( creatingTilesStart( TileCreator*, const QString&, const QString& ) ),
                       m_widget, SLOT( creatingTilesStart( TileCreator*, const QString&, const QString& ) ) );

    m_logZoom  = 0;

    m_widget->goHome();

    // Widget translation
    QString      locale = QLocale::system().name();
    QTranslator  translator;
    translator.load(QString("marblewidget_") + locale);
    QCoreApplication::installTranslator(&translator);

#if 0 // Reenable when the autosettings are actually used.
    // AutoSettings
    AutoSettings* autoSettings = new AutoSettings( this );
#endif

    // FIXME: I suppose this should only exist in MarbleMap
// #if 0
    m_widget->connect( m_model->sunLocator(), SIGNAL( updateSun() ),
                       m_widget, SLOT( updateSun() ) );
    m_widget->connect( m_model->sunLocator(), SIGNAL( centerSun() ),
                       m_widget, SLOT( centerSun() ) );
// #endif
    m_widget->connect( m_model->sunLocator(), SIGNAL( reenableWidgetInput() ),
                       m_widget, SLOT( enableInput() ) );

//    m_widget->connect( m_model->layerDecorator(), SIGNAL( repaintMap() ),
//                       m_widget, SLOT( repaintMap() ) );

    m_widget->connect( m_physics, SIGNAL( valueChanged( qreal ) ),
                        m_widget, SLOT( updateAnimation( qreal ) ) );
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
    if ( d->m_inputhandler )
        delete d->m_inputhandler;

    d->m_inputhandler = handler;

    if ( d->m_inputhandler ) {
        d->m_inputhandler->init( this );
        installEventFilter( d->m_inputhandler );
        connect( d->m_inputhandler, SIGNAL( lmbRequest( int, int ) ),
                 d->m_popupmenu,    SLOT( showLmbMenu( int, int ) ) );
        connect( d->m_inputhandler, SIGNAL( rmbRequest( int, int ) ),
                 d->m_popupmenu,    SLOT( showRmbMenu( int, int ) ) );
        connect( d->m_inputhandler, SIGNAL( mouseClickScreenPosition( int, int) ),
                 this,              SLOT( notifyMouseClick( int, int ) ) );

        connect( d->m_inputhandler, SIGNAL( mouseMoveGeoPosition( QString ) ),
                 this,              SIGNAL( mouseMoveGeoPosition( QString ) ) );
    }
}


void MarbleWidget::setDownloadManager(HttpDownloadManager *downloadManager)
{
    d->m_map->setDownloadManager( downloadManager );
}


Quaternion MarbleWidget::planetAxis() const
{
    return d->m_map->planetAxis();
}


int MarbleWidget::radius() const
{
    return d->m_map->radius();
}

void MarbleWidget::setRadius(const int radius)
{
    d->m_map->setRadius( radius );
}


bool MarbleWidget::needsUpdate() const
{
    return d->m_map->needsUpdate();
}

void MarbleWidget::setNeedsUpdate()
{
    d->m_map->setNeedsUpdate();
}


QAbstractItemModel *MarbleWidget::placeMarkModel() const
{
    return d->m_map->placeMarkModel();
}

QItemSelectionModel *MarbleWidget::placeMarkSelectionModel() const
{
    return d->m_map->placeMarkSelectionModel();
}

double MarbleWidget::moveStep()
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

void MarbleWidget::addPlaceMarkFile( const QString &filename )
{
    d->m_map->addPlaceMarkFile( filename );
    //d->m_model->addPlaceMarkFile( filename );
}

void MarbleWidget::addPlaceMarkData( const QString &data )
{
    d->m_map->addPlaceMarkData( data );
}

QPixmap MarbleWidget::mapScreenShot()
{
    return QPixmap::grabWidget( this );
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
    if ( ! d->m_map->mapCoversViewport() ) {
        setAttribute( Qt::WA_NoSystemBackground, false );
    }
    else {
        setAttribute( Qt::WA_NoSystemBackground, true );
    }

    emit distanceChanged( distanceString() );

    repaint();
    d->setActiveRegion();
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

    repaint();
}

void MarbleWidget::rotateBy( const double& deltaLon, const double& deltaLat)
{
    d->m_map->rotateBy( deltaLon, deltaLat );

    repaint();
}


void MarbleWidget::centerOn( const double& lon, const double& lat, bool animated )
{
    if ( animated )
    {
        d->m_physics->jumpTo( GeoDataPoint( lon, lat, distance(), GeoDataPoint::Degree ) );
    }
    else
    {
        d->m_map->centerOn( lon, lat );
    }

    repaint();
}

void MarbleWidget::centerOn( const QModelIndex& index, bool animated )
{
    if ( animated )
    {
        QItemSelectionModel *selectionModel = d->m_map->model()->placeMarkSelectionModel();
        Q_ASSERT( selectionModel );
    
        selectionModel->clear();
    
        if ( index.isValid() ) {
            const GeoDataPoint targetPosition = index.data( MarblePlacemarkModel::CoordinateRole ).value<GeoDataPoint>();

            d->m_physics->setCurrentPosition( GeoDataPoint( centerLongitude(), centerLatitude(), distance(), GeoDataPoint::Degree ) );
            d->m_physics->jumpTo( targetPosition );

            selectionModel->select( index, QItemSelectionModel::SelectCurrent );
        }
    }
    else
    {
        d->m_map->centerOn( index );
    }

    repaint();
}

void MarbleWidget::centerOn( const GeoDataPoint &position, bool animated )
{
    if ( animated )
    {
        GeoDataPoint targetPosition = position;
        targetPosition.setAltitude( distance() );

        d->m_physics->jumpTo( targetPosition );
    }
    else
    {
        double  lon, lat;
        position.geoCoordinates( lon, lat, GeoDataPoint::Degree );
        d->m_map->setDistance( position.altitude() );
        d->m_map->centerOn( lon, lat );
    }

    if ( d->m_map->mapCoversViewport() ) {
        setAttribute(Qt::WA_NoSystemBackground, true );
    }
    else {
        setAttribute(Qt::WA_NoSystemBackground, false );
    }

    repaint();
}

void MarbleWidget::updateAnimation( qreal updateValue )
{
    GeoDataPoint position = d->m_physics->suggestedPosition();

    if ( updateValue < 1.0 )
    {
        setViewContext( Marble::Animation );
        centerOn( position );
        setViewContext( Marble::Still );
        return;
    }

    centerOn( position );
}

void MarbleWidget::setCenterLatitude( double lat )
{
    centerOn( centerLongitude(), lat );
}

void MarbleWidget::setCenterLongitude( double lon )
{
    centerOn( lon, centerLatitude() );
}

Projection MarbleWidget::projection() const
{
    return d->m_map->projection();
}

void MarbleWidget::setProjection( Projection projection )
{
    d->m_map->setProjection( projection );

    if ( d->m_map->mapCoversViewport()  ) {
        setAttribute( Qt::WA_NoSystemBackground, true );
    }
    else {
        setAttribute( Qt::WA_NoSystemBackground, false );
    }

    repaint();
}

void MarbleWidget::setProjection( int projection )
{
    setProjection( (Projection)( projection ) );
}

void MarbleWidget::home( double &lon, double &lat, int& zoom )
{
    d->m_map->home( lon, lat, zoom );
}

void MarbleWidget::setHome( const double lon, const double lat, const int zoom)
{
    d->m_map->setHome( lon, lat, zoom );
}

void MarbleWidget::setHome(const GeoDataPoint& homePoint, int zoom)
{
    d->m_map->setHome( homePoint, zoom );
}


void MarbleWidget::moveLeft()
{
#if 1
    int polarity = 0;

    if ( northPoleY() != 0 )
        polarity = northPoleY() / abs(northPoleY());

    if ( polarity < 0 )
        rotateBy( +moveStep(), 0 );
    else
        rotateBy( -moveStep(), 0 );
#else
    d->m_map->moveLeft();
#endif
}

void MarbleWidget::moveRight()
{
#if 1
    int polarity = 0;

    if ( northPoleY() != 0 )
        polarity = northPoleY() / abs(northPoleY());

    if ( polarity < 0 )
        rotateBy( -moveStep(), 0 );
    else
        rotateBy( +moveStep(), 0 );
#else
    d->m_map->moveRight();
#endif
}


void MarbleWidget::moveUp()
{
#if 1
    rotateBy( 0, -moveStep() );
#else
    d->m_map->moveUp();
#endif
}

void MarbleWidget::moveDown()
{
#if 1
    rotateBy( 0, +moveStep() );
#else
    d->m_map->moveDown();
#endif
}

void MarbleWidget::leaveEvent (QEvent*)
{
    emit mouseMoveGeoPosition( tr( NOT_AVAILABLE ) );
}

void MarbleWidget::resizeEvent (QResizeEvent*)
{
    d->m_map->setSize( width(), height() );

    //	Redefine the area where the mousepointer becomes a navigationarrow
    d->setActiveRegion();

    if ( d->m_map->mapCoversViewport() ) {
        setAttribute(Qt::WA_NoSystemBackground, true );
    }
    else {
        setAttribute(Qt::WA_NoSystemBackground, false );
    }

    repaint();
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

int MarbleWidget::northPoleZ()
{
    return d->m_map->northPoleZ();
}

bool MarbleWidget::screenCoordinates( const double lon, const double lat,
                                      int& x, int& y )
{
    return d->m_map->screenCoordinates( lon, lat, x, y );
}

bool MarbleWidget::geoCoordinates(const int x, const int y,
                                  double& lon, double& lat,
                                  GeoDataPoint::Unit unit )
{
    return d->m_map->geoCoordinates( x, y, lon, lat, unit );
}

double MarbleWidget::centerLatitude() const
{
    return d->m_map->centerLatitude();
}

double MarbleWidget::centerLongitude() const
{
    return d->m_map->centerLongitude();
}

bool MarbleWidget::globalQuaternion( int x, int y, Quaternion &q)
{
    int  imageHalfWidth  = width() / 2;
    int  imageHalfHeight = height() / 2;

    const double  inverseRadius = 1.0 / (double)(radius());

    if ( radius() > sqrt( (double)(( x - imageHalfWidth ) * ( x - imageHalfWidth )
        + ( y - imageHalfHeight ) * ( y - imageHalfHeight )) ) )
    {
        double qx = inverseRadius * (double)( x - imageHalfWidth );
        double qy = inverseRadius * (double)( y - imageHalfHeight );
        double qr = 1.0 - qy * qy;

        double qr2z = qr - qx * qx;
        double qz = ( qr2z > 0.0 ) ? sqrt( qr2z ) : 0.0;

        Quaternion  qpos( 0.0, qx, qy, qz );
        qpos.rotateAroundAxis( planetAxis() );
        q = qpos;

        return true;
    } else {
        return false;
    }
}

const QRegion MarbleWidget::activeRegion()
{
    return d->m_map->viewParams()->currentProjection()->helper()->activeRegion();
}

void MarbleWidgetPrivate::setActiveRegion()
{
    ViewportParams  *viewport = m_map->viewParams()->viewport();

    viewport->currentProjection()->helper()->setActiveRegion( viewport );
    return;
}

void MarbleWidget::paintEvent(QPaintEvent *evt)
{
    // FIXME: Better way to get the GeoPainter
    bool  doClip = true;
    if ( d->m_map->projection() == Spherical )
        doClip = ( d->m_map->radius() > width() / 2
                   || d->m_map->radius() > height() / 2 );

    // Create a painter that will do the painting.
    GeoPainter painter( this, map()->viewParams()->viewport(),
			map()->viewParams()->mapQuality(), doClip );

    QRect  dirtyRect = evt->rect();
    d->m_map->paint( painter, dirtyRect );
}

void MarbleWidget::customPaint(GeoPainter *painter)
{
    Q_UNUSED( painter );
    /* This is a NOOP in the base class*/
}


void MarbleWidget::goHome()
{
    d->m_map->goHome();

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

void MarbleWidget::changeCurrentPosition( double lon, double lat)
{
    d->m_model->gpsLayer()->changeCurrentPosition( lat, lon );
    repaint();
}

void MarbleWidget::notifyMouseClick( int x, int y)
{
    bool    valid = false;
    double  lon   = 0;
    double  lat   = 0;

    valid = geoCoordinates( x, y, lon, lat, GeoDataPoint::Radian );

    if ( valid ) {
        emit mouseClickGeoPosition( lon, lat, GeoDataPoint::Radian);
    }
}

void MarbleWidget::updateGps()
{
    QRegion temp;
    bool    draw;
    draw = d->m_model->gpsLayer()->updateGps( size(),
                                              d->m_map->viewParams(),
                                              temp );
    if ( draw ){
        update( temp );
    }
    /*
    d->m_model->gpsLayer()->updateGps(
                         size(), radius(),
                              planetAxis() );
    update();*/
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

void MarbleWidget::setPersistentTileCacheLimit( quint64 kiloBytes )
{
    d->m_map->setPersistentTileCacheLimit( kiloBytes );
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

void MarbleWidget::updateRegion( BoundingBox &box )
{
    Q_UNUSED(box);

    //really not sure if this is nessary as its designed for
    //placemark based layers
    setNeedsUpdate();

    /*TODO: write a method for BoundingBox to cacluate the screen
     *region and pass that to update()*/
    update();
}

void MarbleWidget::setDownloadUrl( const QString &url )
{
    setDownloadUrl( QUrl( url ) );
}

void MarbleWidget::setDownloadUrl( const QUrl &url )
{
    d->m_map->setDownloadUrl( url );
}

MapQuality MarbleWidget::mapQuality( ViewContext viewContext )
{
    if ( viewContext == Still )
        return d->m_stillQuality;

    Q_ASSERT( viewContext == Animation );
    return d->m_animationQuality; 
}

void MarbleWidget::setMapQuality( MapQuality mapQuality, ViewContext viewContext )
{
    // FIXME: Rewrite as a switch
    if ( viewContext == Still ) {
        d->m_stillQuality = mapQuality;
    }
    else if ( viewContext == Animation ) {
        d->m_animationQuality = mapQuality;
    }
    map()->viewParams()->setMapQuality( mapQuality ); 
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

double MarbleWidget::distance() const
{
    return map()->distance();
}

void MarbleWidget::setDistance( double distance )
{
    map()->setDistance( distance );
}

QString MarbleWidget::distanceString() const
{
    return QString( "%L1 %2" ).arg( distance(), 8, 'f', 1, QChar(' ') ).arg( tr("km") );
}

void MarbleWidget::updateSun()
{
    // Update the sun shading.
    //SunLocator  *sunLocator = d->m_model->sunLocator();

    //qDebug() << "Updating the sun shading map...";
    d->m_model->update();
    setNeedsUpdate();
    //qDebug() << "Finished updating the sun shading map";
}

void MarbleWidget::centerSun()
{
    SunLocator  *sunLocator = d->m_model->sunLocator();

    double  lon = sunLocator->getLon();
    double  lat = sunLocator->getLat();
    centerOn( lon, lat );

    qDebug() << "centering on Sun at" << lat << lon;
    disableInput();
}

SunLocator* MarbleWidget::sunLocator()
{
    return d->m_model->sunLocator();
}

void MarbleWidget::enableInput()
{
    if ( !d->m_inputhandler ) 
        setInputHandler( new MarbleWidgetDefaultInputHandler );
}

void MarbleWidget::disableInput()
{
    qDebug() << "MarbleWidget::disableInput";
    setInputHandler( 0 );
    setCursor( Qt::ArrowCursor );
}

void MarbleWidget::setProxy( const QString& proxyHost, const quint16 proxyPort )
{
    d->m_proxyHost = proxyHost;
    d->m_proxyPort = proxyPort;

    QNetworkProxy::ProxyType type = QNetworkProxy::HttpProxy;

    // Make sure that no proxy is used for an empty string or the default value: 
    if ( proxyHost.isEmpty() || proxyHost == "http://" )
        type = QNetworkProxy::NoProxy;

    QNetworkProxy proxy( type, d->m_proxyHost, d->m_proxyPort );
    QNetworkProxy::setApplicationProxy( proxy );
    qDebug() << "MarbleWidget::setProxy" << type << d->m_proxyHost << d->m_proxyPort;
}

QString MarbleWidget::proxyHost() const
{
    return d->m_proxyHost;
}

quint16 MarbleWidget::proxyPort() const
{
    return d->m_proxyPort;
}

#include "MarbleWidget.moc"
