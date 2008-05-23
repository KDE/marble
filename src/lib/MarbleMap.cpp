//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2008      Carlos Licea <carlos.licea@kdemail.net>
//


// Own
#include "MarbleMap.h"
#include "MarbleMap_p.h"

// Posix
#include <cmath>

// Qt
#include <QtCore/QCoreApplication>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QTime>
#include <QtGui/QSizePolicy>
#include <QtGui/QRegion>
#include <QtGui/QStyleOptionGraphicsItem>

//#include <QtDBus/QDBusConnection>

// Marble
#include "CrossHairFloatItem.h"
#include "MapScaleFloatItem.h"

#include "AbstractScanlineTextureMapper.h"
#include "BoundingBox.h"
#include "GeoPainter.h"
#include "FileViewModel.h"
#include "FileStoragePolicy.h"
#include "GeoDataPoint.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneZoom.h"
#include "GpxFileViewItem.h"
#include "HttpDownloadManager.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "MarblePlacemarkModel.h"
#include "MeasureTool.h"
#include "MergedLayerDecorator.h"
#include "Quaternion.h"
#include "SunLocator.h"
#include "TextureColorizer.h"
#include "ViewParams.h"
#include "ViewportParams.h"

#include "gps/GpsLayer.h"


#ifdef Q_CC_MSVC
# ifndef KDEWIN_MATH_H
   static long double sqrt(int a) { return sqrt((long double)a); }
# endif
#endif


MarbleMapPrivate::MarbleMapPrivate( MarbleMap *parent )
        : m_parent( parent ),
          m_persistentTileCacheLimit( 1024*1024*300 ), // 300 MB
          m_volatileTileCacheLimit( 1024*3 ) // 3 KB
{
    /* NOOP */
}


MarbleMap::MarbleMap()
    : d( new MarbleMapPrivate( this ) )
{
//    QDBusConnection::sessionBus().registerObject("/marble", this, QDBusConnection::QDBusConnection::ExportAllSlots);

    d->m_model        = new MarbleModel( this );
    d->m_modelIsOwned = true;

    d->construct();
}


MarbleMap::MarbleMap(MarbleModel *model)
    : d( new MarbleMapPrivate( this ) )
{
//    QDBusConnection::sessionBus().registerObject("/marble", this, QDBusConnection::QDBusConnection::ExportAllSlots);

    d->m_model        = model;
    d->m_modelIsOwned = false;

    d->construct();
}

MarbleMap::~MarbleMap()
{
    // Some basic initializations.
    d->m_width  = 0;
    d->m_height = 0;

    setDownloadManager( 0 );

    if ( d->m_modelIsOwned )
        delete d->m_model;
    delete d;
}

void MarbleMapPrivate::construct()
{
    // Some point that tackat defined. :-)
    m_parent->setHome( -9.4, 54.8, 1050 );

    m_parent->connect( m_model, SIGNAL( themeChanged( QString ) ),
                       m_parent, SIGNAL( themeChanged( QString ) ) );
    m_parent->connect( m_model, SIGNAL( modelChanged() ),
                       m_parent, SLOT( updateChangedMap() ) );

    m_parent->connect( m_model, SIGNAL( regionChanged( BoundingBox& ) ) ,
                       m_parent, SLOT( updateRegion( BoundingBox& ) ) );

    // Set background: black.
    // FIXME:
    //setPalette( QPalette ( Qt::black ) );

    // Set whether the black space gets displayed or the earth gets simply 
    // displayed on the map background.
    // FIXME:
    //setAutoFillBackground( true );

    m_justModified = false;

    m_measureTool = new MeasureTool( m_parent );

    m_parent->connect( m_model, SIGNAL( timeout() ),
                       m_parent, SLOT( updateGps() ) );


    m_logzoom  = 0;
    m_zoomStep = 40;

    m_parent->goHome();

    // FloatItems
    m_showFrameRate = false;

    // Map translation
    QString      locale = QLocale::system().name();
    QTranslator  translator;
    translator.load(QString("marblemap_") + locale);
    QCoreApplication::installTranslator(&translator);

#if 0 // Reeneable when the autosettings are actually used

      // AutoSettings
    AutoSettings* autoSettings = new AutoSettings( this );
#endif

//     qDebug() << "sunLocator =" << m_model->sunLocator();
//     m_parent->connect( m_model->sunLocator(), SIGNAL( updateSun() ),
//                        m_parent,              SLOT( updateSun() ) );
//     m_parent->connect( m_model->sunLocator(), SIGNAL( centerSun() ),
//                        m_parent,              SLOT( centerSun() ) );
#if 0
    connect( d->m_model->sunLocator(), SIGNAL( reenableWidgetInput() ),
             this,                     SLOT( enableInput() ) );
#endif
}


// ----------------------------------------------------------------


MarbleModel *MarbleMap::model() const
{
    return d->m_model;
}

ViewParams *MarbleMap::viewParams()
{
    return &d->m_viewParams;
}

MeasureTool *MarbleMap::measureTool()
{
    return d->m_measureTool;
}


void MarbleMap::setSize(int width, int height)
{
    d->m_width  = width;
    d->m_height = height;

    d->doResize();
}

void MarbleMap::setSize(QSize size)
{
    d->m_width  = size.width();
    d->m_height = size.height();

    d->doResize();
}

QSize MarbleMap::size() const
{
    return QSize( d->m_width, d->m_height );
}

int  MarbleMap::width() const
{
    return d->m_width;
}

int  MarbleMap::height() const
{
    return d->m_height;
}


void MarbleMap::setDownloadManager(HttpDownloadManager *downloadManager)
{
    d->m_model->setDownloadManager( downloadManager );
}


Quaternion MarbleMap::planetAxis() const
{
    return d->m_viewParams.planetAxis();
}


int MarbleMap::radius() const
{
    return d->m_viewParams.radius();
}

void MarbleMap::setRadius(const int radius)
{
    d->m_viewParams.setRadius( radius );
}


bool MarbleMap::needsUpdate() const
{
    return ( d->m_justModified
             || d->m_viewParams.radius() != d->m_viewParams.radiusUpdated()
             || !( d->m_viewParams.planetAxis() == d->m_viewParams.planetAxisUpdated() ) );
}

void MarbleMap::setNeedsUpdate()
{
    d->m_justModified = true;
}


QAbstractItemModel *MarbleMap::placeMarkModel() const
{
    return d->m_model->placeMarkModel();
}

QItemSelectionModel *MarbleMap::placeMarkSelectionModel() const
{
    return d->m_model->placeMarkSelectionModel();
}

double MarbleMap::moveStep()
{
    if ( radius() < sqrt( (double)(width() * width() + height() * height()) ) )
	return 180.0 * 0.1;
    else
	return 180.0 * atan( (double)width()
		     / (double)( 2 * radius() ) ) * 0.2;
}

int MarbleMap::zoom() const
{
    return d->m_logzoom;
}

double MarbleMap::centerLatitude() const
{
    // Calculate translation of center point
    double  centerLon;
    double  centerLat;

    d->m_viewParams.centerCoordinates( centerLon, centerLat );
    return centerLat * RAD2DEG;
}

double MarbleMap::centerLongitude() const
{
    // Calculate translation of center point
    double  centerLon;
    double  centerLat;

    d->m_viewParams.centerCoordinates(centerLon, centerLat);
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

void MarbleMap::addPlaceMarkFile( const QString &filename )
{
    d->m_model->addPlaceMarkFile( filename );
}



QPixmap MarbleMap::mapScreenShot()
{
#if 0  //FIXME: reimplement without grabWidget
    return QPixmap::grabWidget( this );
#else
    return QPixmap();
#endif
}

bool MarbleMap::propertyValue( const QString& name ) const
{
    bool value;
    d->m_viewParams.propertyValue( name, value );
    return value;
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
    return d->m_model->layerDecorator()->showClouds();
}

bool MarbleMap::showAtmosphere() const
{
    return d->m_viewParams.showAtmosphere();
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
    return d->m_model->gpsLayer()->visible();
}

bool MarbleMap::showFrameRate() const
{
    return d->m_showFrameRate;
}

quint64 MarbleMap::persistentTileCacheLimit() const
{
    return d->m_persistentTileCacheLimit;
}

quint64 MarbleMap::volatileTileCacheLimit() const
{
    return d->m_volatileTileCacheLimit;
}

void MarbleMap::zoomView(int newZoom)
{
    // Check for under and overflow.
    if ( newZoom < minimumZoom() )
        newZoom = minimumZoom();
    else if ( newZoom > maximumZoom() )
        newZoom = maximumZoom();

    // Prevent infinite loops.
    if ( newZoom  == d->m_logzoom )
	return;
    d->m_logzoom = newZoom;
    setRadius( d->fromLogScale( newZoom ) );

    // Clear canvas if the globe is visible as a whole or if the globe
    // does shrink.
    if ( ! globeCoversImage() 
         || projection() != Spherical )
    {
        d->m_viewParams.canvasImage()->fill( Qt::black );
    }

    // We don't do this on every paintEvent to improve performance.
    // Redrawing the atmosphere is only needed if the size of the 
    // globe changes.
    if ( d->m_viewParams.showAtmosphere() ) {
        d->drawAtmosphere();
    }

    emit zoomChanged( newZoom );
}


void MarbleMap::zoomViewBy( int zoomStep )
{
    zoomView( d->toLogScale( radius() ) + zoomStep );
}


void MarbleMap::zoomIn()
{
    zoomViewBy( d->m_zoomStep );
}

void MarbleMap::zoomOut()
{
    zoomViewBy( -d->m_zoomStep );
}

void MarbleMap::rotateTo(const Quaternion& quat)
{
    d->m_viewParams.setPlanetAxis( quat );

    // This method doesn't force a repaint of the view on purpose!
    // See header file.
}


void MarbleMap::rotateBy(const Quaternion& incRot)
{
    d->m_viewParams.setPlanetAxis( incRot * d->m_viewParams.planetAxis() );
}

void MarbleMap::rotateBy( const double& deltaLon, const double& deltaLat)
{
    Quaternion  rotPhi( 1.0, deltaLat / 180.0, 0.0, 0.0 );
    Quaternion  rotTheta( 1.0, 0.0, deltaLon / 180.0, 0.0 );

    Quaternion  axis = d->m_viewParams.planetAxis();
    axis = rotTheta * axis;
    axis *= rotPhi;
    axis.normalize();
    d->m_viewParams.setPlanetAxis( axis );
}


void MarbleMap::centerOn(const double& lon, const double& lat)
{
    Quaternion  quat;
    quat.createFromEuler( -lat * DEG2RAD, lon * DEG2RAD, 0.0 );
    d->m_viewParams.setPlanetAxis( quat );
}

void MarbleMap::centerOn(const QModelIndex& index)
{
    QItemSelectionModel *selectionModel = d->m_model->placeMarkSelectionModel();
    Q_ASSERT( selectionModel );

    selectionModel->clear();

    if ( index.isValid() ) {
        const GeoDataPoint point = index.data( MarblePlacemarkModel::CoordinateRole ).value<GeoDataPoint>();
  
        double  lon;
        double  lat;
        point.geoCoordinates( lon, lat );

        centerOn( lon * RAD2DEG, lat * RAD2DEG );

        selectionModel->select( index, QItemSelectionModel::SelectCurrent );
        d->m_crosshair.setEnabled( true );
    }
    else
        d->m_crosshair.setEnabled( false );
}


void MarbleMap::setCenterLatitude( double lat )
{
    centerOn( centerLongitude(), lat );
}

void MarbleMap::setCenterLongitude( double lon )
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

    // Redraw the background if necessary
    if ( !globeCoversImage() 
         || d->m_viewParams.projection() != Spherical )
    {
        d->m_viewParams.canvasImage()->fill( Qt::black );
    }
 
    if ( d->m_viewParams.showAtmosphere() ) {
        d->drawAtmosphere();
    }

    d->m_model->setupTextureMapper( projection );

    // Update texture map during the repaint that follows:
    setNeedsUpdate();
}

void MarbleMap::home( double &lon, double &lat, int& zoom )
{
    d->m_homePoint.geoCoordinates( lon, lat, GeoDataPoint::Degree );
    zoom = d->m_homeZoom;
}

void MarbleMap::setHome( const double lon, const double lat, const int zoom)
{
    d->m_homePoint = GeoDataPoint( lon, lat, 0, GeoDataPoint::Degree );
    d->m_homeZoom = zoom;
}

void MarbleMap::setHome(const GeoDataPoint& homePoint, int zoom)
{
    d->m_homePoint = homePoint;
    d->m_homeZoom = zoom;
}


void MarbleMap::moveLeft()
{
    int polarity = 0;

    if ( northPoleY() != 0 )
        polarity = northPoleY() / abs(northPoleY());

    if ( polarity < 0 )
        rotateBy( +moveStep(), 0 );
    else
        rotateBy( -moveStep(), 0 );
}

void MarbleMap::moveRight()
{
    int polarity = 0;

    if ( northPoleY() != 0 )
        polarity = northPoleY() / abs(northPoleY());

    if ( polarity < 0 )
        rotateBy( -moveStep(), 0 );
    else
        rotateBy( +moveStep(), 0 );
}


void MarbleMap::moveUp()
{
    rotateBy( 0, -moveStep() );
}

void MarbleMap::moveDown()
{
    rotateBy( 0, +moveStep() );
}

// Used to be resizeEvent()
void MarbleMapPrivate::doResize()
{
    QSize size(m_parent->width(), m_parent->height());
    m_viewParams.viewport()->setSize( size );
    // Recreate the canvas image with the new size.
    m_viewParams.setCanvasImage( new QImage( m_parent->width(), m_parent->height(),
                                             QImage::Format_ARGB32_Premultiplied ));

    // Repaint the background if necessary
    if ( ! m_parent->globeCoversImage()
         || m_viewParams.projection() != Spherical )
    {
        m_viewParams.canvasImage()->fill( Qt::black );
    }

    if ( m_viewParams.showAtmosphere() ) {
        drawAtmosphere();
    }

    // Recreate the 
    m_viewParams.setCoastImage( new QImage( m_parent->width(), m_parent->height(),
                                            QImage::Format_ARGB32_Premultiplied ));

    m_justModified = true;
}

int MarbleMap::northPoleY()
{
    Quaternion  northPole     = GeoDataPoint( 0.0, M_PI * 0.5 ).quaternion();
    Quaternion  invPlanetAxis = d->m_viewParams.planetAxis().inverse();

    northPole.rotateAroundAxis( invPlanetAxis );
    return (int)( d->m_viewParams.radius() * northPole.v[Q_Y] );
}

int MarbleMap::northPoleZ()
{
    Quaternion  northPole     = GeoDataPoint( 0.0, M_PI * 0.5 ).quaternion();
    Quaternion  invPlanetAxis = d->m_viewParams.planetAxis().inverse();

    northPole.rotateAroundAxis( invPlanetAxis );

    return (int)( d->m_viewParams.radius() * northPole.v[Q_Z] );
}


bool MarbleMap::screenCoordinates( const double lon, const double lat,
                                   int& x, int& y )
{
    return d->m_viewParams.currentProjection()
        ->screenCoordinates( lon * DEG2RAD, lat * DEG2RAD,
                             d->m_viewParams.viewport(),
                             x, y );
}

bool MarbleMap::geoCoordinates( const int x, const int y,
                                double& lon, double& lat,
                                GeoDataPoint::Unit unit )
{
    return d->m_viewParams.currentProjection()
        ->geoCoordinates( x, y, d->m_viewParams.viewport(),
                          lon, lat, unit );
}

bool MarbleMap::globalQuaternion( int x, int y, Quaternion &q)
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




void MarbleMap::rotateTo( const double& lon, const double& lat, const double& psi)
{
    Quaternion  quat;
    quat.createFromEuler( -lat * DEG2RAD,   // "phi"
                           lon * DEG2RAD,   // "theta"
                           psi * DEG2RAD );
    d->m_viewParams.setPlanetAxis( quat );
}

void MarbleMap::rotateTo(const double& lon, const double& lat)
{
    Quaternion  quat;
    quat.createFromEuler( -lat * DEG2RAD, lon  * DEG2RAD, 0.0 );
    d->m_viewParams.setPlanetAxis( quat );
}


void MarbleMapPrivate::drawAtmosphere()
{
    // Only draw an atmosphere if projection is spherical
    if ( m_viewParams.projection() != Spherical )
        return;

    // No use to draw atmosphere if it's not visible in the area. 
    if ( m_parent->globeCoversImage() )
        return;

    // Ok, now we know that at least a little of the atmosphere is
    // visible, if nothing else in the corners.  Draw the atmosphere
    // by using a circular gradient.  This is a pure visual effect and
    // has nothing to do with real physics.

    int  imageHalfWidth  = m_parent->width() / 2;
    int  imageHalfHeight = m_parent->height() / 2;

    // Recalculate the atmosphere effect and paint it to canvasImage.
    QRadialGradient grad1( QPointF( imageHalfWidth, imageHalfHeight ),
                           1.05 * m_parent->radius() );
    grad1.setColorAt( 0.91, QColor( 255, 255, 255, 255 ) );
    grad1.setColorAt( 1.00, QColor( 255, 255, 255, 0 ) );

    QBrush    brush1( grad1 );
    QPen      pen1( Qt::NoPen );
    QPainter  painter( m_viewParams.canvasImage() );
    painter.setBrush( brush1 );
    painter.setPen( pen1 );
    painter.setRenderHint( QPainter::Antialiasing, false );
    painter.drawEllipse( imageHalfWidth  - (int)( (double)(m_parent->radius()) * 1.05 ),
                         imageHalfHeight - (int)( (double)(m_parent->radius()) * 1.05 ),
                         (int)( 2.1 * (double)(m_parent->radius()) ),
                         (int)( 2.1 * (double)(m_parent->radius()) ) );
}


void MarbleMapPrivate::drawFog( QPainter &painter )
{
    if ( m_viewParams.projection() != Spherical)
        return;

    // No use to draw the fog if it's not visible in the area. 
    if ( m_parent->globeCoversImage() )
        return;

    int  imgWidth2  = m_parent->width() / 2;
    int  imgHeight2 = m_parent->height() / 2;

    // Recalculate the atmosphere effect and paint it to canvasImage.
    QRadialGradient grad1( QPointF( imgWidth2, imgHeight2 ), m_parent->radius() );

    // FIXME: Add a cosine relationship
    grad1.setColorAt( 0.85, QColor( 255, 255, 255, 0 ) );
    grad1.setColorAt( 1.00, QColor( 255, 255, 255, 64 ) );

    QBrush    brush1( grad1 );
    QPen      pen1( Qt::NoPen );

    painter.save();

    painter.setBrush( brush1 );
    painter.setPen( pen1 );
    painter.setRenderHint( QPainter::Antialiasing, false );

    // FIXME: Cut out what's really needed
    painter.drawEllipse( imgWidth2  - m_parent->radius(),
                         imgHeight2 - m_parent->radius(),
                         2 * m_parent->radius(),
                         2 * m_parent->radius() );

    painter.restore();
}

void MarbleMapPrivate::setBoundingBox()
{
    QVector<QPointF>  points;
    Quaternion        temp;

    if ( m_parent->globalQuaternion( 0, 0, temp) ) {
        points.append( QPointF( temp.v[Q_X], temp.v[Q_Y]) );
    }
    if ( m_parent->globalQuaternion( m_parent->width() / 2, 0, temp ) ) {
        points.append( QPointF( temp.v[Q_X], temp.v[Q_Y]) );
    }

    if ( m_parent->globalQuaternion( m_parent->width(), 0, temp ) ) {
        points.append( QPointF( temp.v[Q_X], temp.v[Q_Y]) );
    }
    if ( m_parent->globalQuaternion( 0, m_parent->height(), temp ) ) {
        points.append( QPointF( temp.v[Q_X], temp.v[Q_Y]) );
    }

    if ( m_parent->globalQuaternion( m_parent->width()/2, m_parent->height(), temp ) ) {
        points.append( QPointF( temp.v[Q_X], temp.v[Q_Y]) );
    }

    if ( m_parent->globalQuaternion( m_parent->width(), m_parent->height(), temp ) ) {
        points.append( QPointF( temp.v[Q_X], temp.v[Q_Y]) );
    }

    m_viewParams.viewport()->m_boundingBox = BoundingBox( points );
}


// Used to be paintEvent()
void MarbleMap::paint(GeoPainter &painter, QRect &dirtyRect) 
{
    if ( !d->m_viewParams.mapTheme() ) {
        qDebug() << "No theme yet!";
        return;
    }
        
    QTime t;
    t.start();

    bool  doClip = false;
    if ( d->m_viewParams.projection() == Spherical )
        doClip = ( d->m_viewParams.radius() > d->m_viewParams.canvasImage()->width() / 2
                   || d->m_viewParams.radius() > d->m_viewParams.canvasImage()->height() / 2 );

    // Create a painter that will do the painting.
#if 0
    GeoPainter painter( this, doClip );
#endif
    // 1. Paint the globe itself.
#if 0
    QRect  dirtyRect = evt->rect();
#endif
    d->m_model->paintGlobe( &painter,
                            width(), height(), &d->m_viewParams,
                            needsUpdate()
                            || d->m_viewParams.canvasImage()->isNull(),
                            dirtyRect );
    // FIXME: this is ugly, add method updatePlanetAxis() to ViewParams
    d->m_viewParams.setPlanetAxisUpdated( d->m_viewParams.planetAxis() );
    // FIXME: this is ugly, add method updateRadius() to ViewParams
    d->m_viewParams.setRadiusUpdated( d->m_viewParams.radius() );
    d->m_justModified                   = false;

    // FIXME: This is really slow. That's why we defer this to
    //        PrintQuality. Either cache on a pixmap - or maybe
    //        better: Add to GlobeScanlineTextureMapper.

    if ( d->m_viewParams.viewport()->mapQuality() == Marble::Print )
        d->drawFog(painter);

    customPaint( &painter );

    int transparency = ( d->m_viewParams.viewport()->mapQuality() == Marble::Low ) ? 255 : 192;
//    d->m_compass.setTransparency( transparency );
    d->m_mapscale.setTransparency( transparency );

    // 2. Paint the compass

    bool showCompass, showScaleBar;

    d->m_viewParams.propertyValue( "compass", showCompass );
    d->m_viewParams.propertyValue( "scalebar", showScaleBar );
/*
    if ( showCompass )
        painter.drawPixmap( d->m_viewParams.canvasImage()->width() - 60, 10,
                            d->m_compass.drawCompassPixmap( d->m_viewParams.canvasImage()->width(),
                                                            d->m_viewParams.canvasImage()->height(),
                                                            northPoleY(), d->m_viewParams.projection() ) );
*/
    // 3. Paint the scale.
    if ( showScaleBar )
        painter.drawPixmap( 10, d->m_viewParams.canvasImage()->height() - 40,
                            d->m_mapscale.drawScaleBarPixmap( radius(),
                                                              d->m_viewParams.canvasImage()->width() / 2 - 20 ) );

    // 4. Paint the crosshair.
    d->m_crosshair.paint( &painter,
                          d->m_viewParams.canvasImage()->width(),
                          d->m_viewParams.canvasImage()->height() );

    // 5. Paint measure points if there are any.

    bool antialiased = false;

    if (   d->m_viewParams.viewport()->mapQuality() == Marble::High
        || d->m_viewParams.viewport()->mapQuality() == Marble::Print ) {
            antialiased = true;
    }

    d->m_measureTool->paint( &painter, d->m_viewParams.viewport(), antialiased );

    // Set the Bounding Box
    d->setBoundingBox();

    double fps = 1000.0 / (double)( t.elapsed() );

    if ( d->m_showFrameRate == true ) {
        QString fpsString = QString( "Speed: %1 fps" ).arg( fps, 5, 'f', 1, QChar(' ') );

        QPoint fpsLabelPos( 10, 20 );

        painter.setFont( QFont( "Sans Serif", 10 ) );

        painter.setPen( Qt::black );
        painter.setBrush( Qt::black );
        painter.drawText( fpsLabelPos, fpsString );

        painter.setPen( Qt::white );
        painter.setBrush( Qt::white );
        painter.drawText( fpsLabelPos.x() - 1, fpsLabelPos.y() - 1, fpsString );
    }

    emit framesPerSecond( fps );
}

void MarbleMap::customPaint(GeoPainter *painter)
{
    Q_UNUSED( painter );
    /* This is a NOOP */
}

#if 0
void MarbleMap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{
    Q_UNUSED( painter );
    Q_UNUSED( option );
    Q_UNUSED( widget );
}
#endif

void MarbleMap::goHome()
{
    // d->m_model->rotateTo(0, 0);
    double  homeLon = 0;
    double  homeLat = 0;
    d->m_homePoint.geoCoordinates( homeLon, homeLat );

    rotateTo( homeLon * RAD2DEG, homeLat * RAD2DEG );

    zoomView( d->m_homeZoom ); // default 1050
}

QString MarbleMap::mapThemeId() const
{
    return d->m_model->mapThemeId();
}

void MarbleMap::setMapThemeId( const QString& mapThemeId )
{
    if ( !mapThemeId.isEmpty() && mapThemeId == d->m_model->mapThemeId() )
        return;

    d->m_viewParams.setMapThemeId( mapThemeId );
    GeoSceneDocument *mapTheme = d->m_viewParams.mapTheme();

    d->m_model->setMapTheme( mapTheme, d->m_viewParams.projection() );

    // Update texture map during the repaint that follows:
    setNeedsUpdate();
}

void MarbleMap::setPropertyValue( const QString& name, bool value )
{
    qDebug() << "In MarbleMap the property " << name << "was set to " << value;
    d->m_viewParams.setPropertyValue( name, value );
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
    // Quick and dirty way to force a whole update of the view
    d->doResize();
}

void MarbleMap::setShowClouds( bool visible )
{
    d->m_model->layerDecorator()->setShowClouds( visible );
    d->m_model->update();
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

void MarbleMap::setShowElevationModel( bool visible )
{
    d->m_viewParams.setShowElevationModel( visible );
    // Update texture map during the repaint that follows:
    setNeedsUpdate();
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

void MarbleMap::setShowGps( bool visible )
{
    d->m_viewParams.setShowGps( visible );
}

void MarbleMap::changeCurrentPosition( double lon, double lat)
{
    d->m_model->gpsLayer()->changeCurrentPosition( lat, lon );
}

void MarbleMap::notifyMouseClick( int x, int y)
{
    bool    valid = false;
    double  lon   = 0;
    double  lat   = 0;

    valid = geoCoordinates( x, y, lon, lat, GeoDataPoint::Radian );

    if ( valid ) {
        emit mouseClickGeoPosition( lon, lat, GeoDataPoint::Radian);
    }
}

void MarbleMap::updateGps()
{
    QRegion temp;
    bool    draw;
    draw = d->m_model->gpsLayer()->updateGps( size(),&d->m_viewParams, temp );
#if 0  // FIXME: move to MarbleWidget?
    if ( draw ) {
        update(temp);
    }
#endif

#if 0
    d->m_model->gpsLayer()->updateGps( size(), radius(), planetAxis() );
    update();
#endif
}

void MarbleMap::openGpxFile(QString &filename)
{
#ifndef KML_GSOC
    d->m_model->gpsLayer()->loadGpx( filename );
#else
    GpxFileViewItem* item = new GpxFileViewItem( new GpxFile( filename ) );
    d->m_model->fileViewModel()->append( item );
#endif
}

GpxFileModel *MarbleMap::gpxFileModel()
{
    return d->m_model->gpxFileModel();
}

FileViewModel* MarbleMap::fileViewModel() const
{
    return d->m_model->fileViewModel();
}

void MarbleMap::setPersistentTileCacheLimit( quint64 kiloBytes )
{
    d->m_persistentTileCacheLimit = kiloBytes;
    // TODO: trigger update
}

void MarbleMap::setVolatileTileCacheLimit( quint64 kilobytes )
{
    qDebug() << "kiloBytes" << kilobytes;
    d->m_volatileTileCacheLimit = kilobytes;
    d->m_model->setVolatileTileCacheLimit( kilobytes );
}

void MarbleMap::updateChangedMap()
{
    // Update texture map during the repaint that follows:
    setNeedsUpdate();
}

void MarbleMap::updateRegion( BoundingBox &box )
{
    Q_UNUSED(box);
    //really not sure if this is nessary as its designed for
    //placemark based layers
    setNeedsUpdate();

    /*TODO: write a method for BoundingBox to cacluate the screen
     *region and pass that to update()*/
}

void MarbleMap::setDownloadUrl( const QString &url )
{
    setDownloadUrl( QUrl( url ) );
}

void MarbleMap::setDownloadUrl( const QUrl &url ) {
    HttpDownloadManager *downloadManager = d->m_model->downloadManager();
    if ( downloadManager != 0 )
        downloadManager->setServerUrl( url );
    else
    {
        downloadManager = new HttpDownloadManager( url,
                                                   new FileStoragePolicy( MarbleDirs::localPath() ) );
        d->m_model->setDownloadManager( downloadManager );
    }
}

QString MarbleMap::distanceString() const
{
    const double VIEW_ANGLE = 110.0;

    // Due to Marble's orthographic projection ("we have no focus")
    // it's actually not possible to calculate a "real" distance.
    // Additionally the viewing angle of the earth doesn't adjust to
    // the window's size.
    //
    // So the only possible workaround is to come up with a distance
    // definition which gives a reasonable approximation of
    // reality. Therefore we assume that the average window width
    // (about 800 pixels) equals the viewing angle of a human being.
    //
    double distance = ( EARTH_RADIUS * 0.4
			/ (double)( radius() )
			/ tan( 0.5 * VIEW_ANGLE * DEG2RAD ) );

    return QString( "%L1 %2" ).arg( distance, 8, 'f', 1, QChar(' ') ).arg( tr("km") );
}


bool MarbleMap::globeCoversImage()
{
    // This first test is a quick one that will catch all really big
    // radii and prevent overflow in the real test.
    if ( radius() > width() + height() )
        return true;

    // This is the real test.  The 4 is because we are really
    // comparing to width/2 and height/2.
    if ( 4 * radius() * radius() >= width() * width() + height() * height() )
        return true;

    return false;
}



void MarbleMap::updateSun()
{
    // Update the sun shading.
    //SunLocator  *sunLocator = d->m_model->sunLocator();

    //qDebug() << "Updating the sun shading map...";
    d->m_model->update();
    setNeedsUpdate();
    //qDebug() << "Finished updating the sun shading map";
}

void MarbleMap::centerSun()
{
    SunLocator  *sunLocator = d->m_model->sunLocator();

    double  lon = sunLocator->getLon();
    double  lat = sunLocator->getLat();
    centerOn( lon, lat );

    qDebug() << "Centering on Sun at " << lat << lon;
}

SunLocator* MarbleMap::sunLocator()
{
    return d->m_model->sunLocator();
}


#include "MarbleMap.moc"
