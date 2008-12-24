//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2009 Torsten Rahn <tackat@kde.org>"
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
#include <QtCore/QDebug>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QTime>
#include <QtGui/QItemSelectionModel>
#include <QtGui/QSizePolicy>
#include <QtGui/QRegion>
#include <QtGui/QStyleOptionGraphicsItem>

//#include <QtDBus/QDBusConnection>

// Marble
#include "AbstractProjection.h"
#include "AbstractScanlineTextureMapper.h"
#include "BoundingBox.h"
#include "GeoPainter.h"
#include "FileViewModel.h"
#include "FileStoragePolicy.h"
#include "GeoDataFeature.h"
#include "GeoDataCoordinates.h"
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

using namespace Marble;

#ifdef Q_CC_MSVC
# ifndef KDEWIN_MATH_H
   static long double sqrt(int a) { return sqrt((long double)a); }
# endif
#endif


MarbleMapPrivate::MarbleMapPrivate( MarbleMap *parent )
        : m_parent( parent ),
          m_persistentTileCacheLimit( 1024*1024*300 ), // 300 MB
          m_volatileTileCacheLimit( 1024*1024*30 ) // 30 MB
{
    /* NOOP */
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

    m_justModified = false;

    m_measureTool = new MeasureTool( m_model, m_parent );

    m_parent->connect( m_model, SIGNAL( timeout() ),
                       m_parent, SLOT( updateGps() ) );


    m_logzoom  = 0;
    m_zoomStep = 40;

    m_parent->goHome();

    // FloatItems
    m_showFrameRate = false;


    m_parent->connect( m_model->sunLocator(), SIGNAL( updateSun() ),
                        m_parent,              SLOT( updateSun() ) );
    m_parent->connect( m_model->sunLocator(), SIGNAL( centerSun() ),
                        m_parent,              SLOT( centerSun() ) );
}

// Used to be resizeEvent()
void MarbleMapPrivate::doResize()
{
    QSize size(m_parent->width(), m_parent->height());
    m_viewParams.viewport()->setSize( size );
    // Recreate the canvas image with the new size.
    m_viewParams.setCanvasImage( new QImage( m_parent->width(), m_parent->height(),
                                             QImage::Format_ARGB32_Premultiplied ));

    if ( m_viewParams.showAtmosphere() ) {
        drawAtmosphere();
    }

    // Recreate the 
    m_viewParams.setCoastImage( new QImage( m_parent->width(), m_parent->height(),
                                            QImage::Format_ARGB32_Premultiplied ));

    m_justModified = true;
}

void  MarbleMapPrivate::paintMarbleSplash( GeoPainter &painter, QRect &dirtyRect )
{
    painter.save();

    QPixmap logoPixmap( MarbleDirs::path( "svg/marble-logo-inverted-72dpi.png" ) );

    if ( logoPixmap.width() > m_parent->width() * 0.7 || logoPixmap.height() > m_parent->height() * 0.7 )
    {
        logoPixmap = logoPixmap.scaled( QSize( m_parent->width(), m_parent->height() ) * 0.7, Qt::KeepAspectRatio, Qt::SmoothTransformation );
    }

    QPoint logoPosition( ( m_parent->width()  - logoPixmap.width() ) / 2 , 
                            ( m_parent->height() - logoPixmap.height() ) / 2 ); 
    painter.drawPixmap( logoPosition, logoPixmap );

    QString message = ""; // "Please assign a map theme!";

    painter.setPen( Qt::white );

    int yTop = logoPosition.y() + logoPixmap.height() + 10;
    QRect textRect( 0, yTop,
                    m_parent->width(), m_parent->height() - yTop );
    painter.drawText( textRect, Qt::AlignHCenter | Qt::AlignTop, message ); 

    painter.restore();
}

void MarbleMapPrivate::drawAtmosphere()
{
    // Only draw an atmosphere if planet is earth
    GeoSceneDocument * mapTheme = m_viewParams.mapTheme();
    if ( mapTheme ) {
        if ( mapTheme->head()->target() != "earth" )
            return;
    }

    // Only draw an atmosphere if projection is spherical
    if ( m_viewParams.projection() != Spherical )
        return;

    // No use to draw atmosphere if it's not visible in the area. 
    if ( m_viewParams.viewport()->mapCoversViewport() )
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
    painter.drawEllipse( imageHalfWidth  - (int)( (qreal)(m_parent->radius()) * 1.05 ),
                         imageHalfHeight - (int)( (qreal)(m_parent->radius()) * 1.05 ),
                         (int)( 2.1 * (qreal)(m_parent->radius()) ),
                         (int)( 2.1 * (qreal)(m_parent->radius()) ) );
}

void MarbleMapPrivate::drawFog( QPainter &painter )
{
    if ( m_viewParams.projection() != Spherical)
        return;

    // No use to draw the fog if it's not visible in the area. 
    if ( m_viewParams.viewport()->mapCoversViewport() )
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

    m_viewParams.viewport()->setBoundingBox( BoundingBox( points ) );
}

void MarbleMapPrivate::paintGround( GeoPainter &painter, QRect &dirtyRect )
{
    if ( !m_viewParams.mapTheme() ) 
    {
        qDebug() << "No theme yet!";
        paintMarbleSplash( painter, dirtyRect );
        return;
    }

    bool  doClip = false;
    if ( m_viewParams.projection() == Spherical )
        doClip = ( m_viewParams.radius() > m_viewParams.canvasImage()->width() / 2
                   || m_viewParams.radius() > m_viewParams.canvasImage()->height() / 2 );

    m_model->paintGlobe( &painter,
                            m_parent->width(), m_parent->height(), &m_viewParams,
                            m_parent->needsUpdate()
                            || m_viewParams.canvasImage()->isNull(),
                            dirtyRect );
    // FIXME: this is ugly, add method updatePlanetAxis() to ViewParams
    m_viewParams.setPlanetAxisUpdated( m_viewParams.planetAxis() );
    // FIXME: this is ugly, add method updateRadius() to ViewParams
    m_viewParams.setRadiusUpdated( m_viewParams.radius() );
    m_justModified                   = false;

    // FIXME: This is really slow. That's why we defer this to
    //        PrintQuality. Either cache on a pixmap - or maybe
    //        better: Add to GlobeScanlineTextureMapper.

    if ( m_viewParams.mapQuality() == Marble::Print )
        drawFog(painter);
}

void MarbleMapPrivate::paintOverlay( GeoPainter &painter, QRect &dirtyRect)
{
    if ( !m_viewParams.mapTheme() ) 
    {
        return;
    }

    // FIXME: Add this stuff into the Layermanager as something to be 
    // called before the float items.

    bool antialiased = false;

    if (   m_viewParams.mapQuality() == Marble::High
        || m_viewParams.mapQuality() == Marble::Print ) {
            antialiased = true;
    }

    m_measureTool->paint( &painter, m_viewParams.viewport(), antialiased );

    // FIXME: Get rid of this:
    // Set the Bounding Box 
    setBoundingBox();
}

void MarbleMapPrivate::paintFps( GeoPainter &painter, QRect &dirtyRect, qreal fps)
{
    Q_UNUSED(dirtyRect);

    if ( m_showFrameRate == true ) {
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
}


// ----------------------------------------------------------------


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

void MarbleMap::setRadius(int radius)
{
    d->m_viewParams.setRadius( radius );

    if ( !mapCoversViewport() ) {
        setNeedsUpdate();
    }
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

qreal MarbleMap::moveStep()
{
    if ( radius() < sqrt( (qreal)(width() * width() + height() * height()) ) )
	return 180.0 * 0.1;
    else
	return 180.0 * atan( (qreal)width()
		     / (qreal)( 2 * radius() ) ) * 0.2;
}

int MarbleMap::zoom() const
{
    return d->m_logzoom;
}


qreal MarbleMap::distance() const
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

    const qreal VIEW_ANGLE = 110.0;

    return ( model()->planetRadius() * 0.4
            / (qreal)( radius() )
            / tan( 0.5 * VIEW_ANGLE * DEG2RAD ) );
}

void MarbleMap::setDistance( qreal distance )
{
    const qreal VIEW_ANGLE = 110.0;

    setRadius( (int)( model()->planetRadius() * 0.4
            / distance
            / tan( 0.5 * VIEW_ANGLE * DEG2RAD ) ) );
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

void MarbleMap::addPlaceMarkData( const QString &data )
{
    d->m_model->addPlaceMarkData( data );
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
    return d->m_model->layerDecorator()->showClouds();
}

bool MarbleMap::showAtmosphere() const
{
    return d->m_viewParams.showAtmosphere();
}

bool MarbleMap::showCrosshairs() const
{
    return propertyValue( "crosshairs" );
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

void MarbleMap::rotateBy(const Quaternion& incRot)
{
    d->m_viewParams.setPlanetAxis( incRot * d->m_viewParams.planetAxis() );
}

void MarbleMap::rotateBy( const qreal& deltaLon, const qreal& deltaLat)
{
    Quaternion  rotPhi( 1.0, deltaLat / 180.0, 0.0, 0.0 );
    Quaternion  rotTheta( 1.0, 0.0, deltaLon / 180.0, 0.0 );

    Quaternion  axis = d->m_viewParams.planetAxis();
    axis = rotTheta * axis;
    axis *= rotPhi;
    axis.normalize();
    d->m_viewParams.setPlanetAxis( axis );
}


void MarbleMap::centerOn(const qreal& lon, const qreal& lat)
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
        const GeoDataCoordinates point = index.data( MarblePlacemarkModel::CoordinateRole ).value<GeoDataCoordinates>();
  
        qreal  lon;
        qreal  lat;
        point.geoCoordinates( lon, lat );

        centerOn( lon * RAD2DEG, lat * RAD2DEG );

        selectionModel->select( index, QItemSelectionModel::SelectCurrent );
//        Maybe add some similar functionality later on again:
//        d->m_crosshair.setEnabled( true );
    }
//    else
//        d->m_crosshair.setEnabled( false );
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
 
    if ( d->m_viewParams.showAtmosphere() ) {
        d->drawAtmosphere();
    }

    d->m_model->setupTextureMapper( projection );

    // Update texture map during the repaint that follows:
    setNeedsUpdate();
}

void MarbleMap::home( qreal &lon, qreal &lat, int& zoom )
{
    d->m_homePoint.geoCoordinates( lon, lat, GeoDataCoordinates::Degree );
    zoom = d->m_homeZoom;
}

void MarbleMap::setHome( qreal lon, qreal lat, int zoom)
{
    d->m_homePoint = GeoDataCoordinates( lon, lat, 0, GeoDataCoordinates::Degree );
    d->m_homeZoom = zoom;
}

void MarbleMap::setHome(const GeoDataCoordinates& homePoint, int zoom)
{
    d->m_homePoint = homePoint;
    d->m_homeZoom = zoom;
}


void MarbleMap::moveLeft()
{
    int polarity = viewParams()->viewport()->polarity();

    if ( polarity < 0 )
        rotateBy( +moveStep(), 0 );
    else
        rotateBy( -moveStep(), 0 );
}

void MarbleMap::moveRight()
{
    int polarity = viewParams()->viewport()->polarity();

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

int MarbleMap::northPoleY()
{
    Quaternion  northPole     = GeoDataCoordinates( 0.0, M_PI * 0.5 ).quaternion();
    Quaternion  invPlanetAxis = d->m_viewParams.planetAxis().inverse();

    northPole.rotateAroundAxis( invPlanetAxis );
    return (int)( d->m_viewParams.radius() * northPole.v[Q_Y] );
}

bool MarbleMap::screenCoordinates( qreal lon, qreal lat,
                                   int& x, int& y )
{
    return d->m_viewParams.currentProjection()
        ->screenCoordinates( lon * DEG2RAD, lat * DEG2RAD,
                             d->m_viewParams.viewport(),
                             x, y );
}

bool MarbleMap::geoCoordinates( int x, int y,
                                qreal& lon, qreal& lat,
                                GeoDataCoordinates::Unit unit )
{
    return d->m_viewParams.currentProjection()
        ->geoCoordinates( x, y, d->m_viewParams.viewport(),
                          lon, lat, unit );
}

bool MarbleMap::globalQuaternion( int x, int y, Quaternion &q)
{
    int  imageHalfWidth  = width() / 2;
    int  imageHalfHeight = height() / 2;

    const qreal  inverseRadius = 1.0 / (qreal)(radius());

    if ( radius() > sqrt( (qreal)(( x - imageHalfWidth ) * ( x - imageHalfWidth )
        + ( y - imageHalfHeight ) * ( y - imageHalfHeight )) ) )
    {
        qreal qx = inverseRadius * (qreal)( x - imageHalfWidth );
        qreal qy = inverseRadius * (qreal)( y - imageHalfHeight );
        qreal qr = 1.0 - qy * qy;

        qreal qr2z = qr - qx * qx;
        qreal qz = ( qr2z > 0.0 ) ? sqrt( qr2z ) : 0.0;

        Quaternion  qpos( 0.0, qx, qy, qz );
        qpos.rotateAroundAxis( planetAxis() );
        q = qpos;

        return true;
    } else {
        return false;
    }
}

// Used to be paintEvent()
void MarbleMap::paint(GeoPainter &painter, QRect &dirtyRect) 
{
    QTime t;
    t.start();

    d->paintGround(painter, dirtyRect);
    customPaint( &painter );
    d->paintOverlay(painter, dirtyRect);

    qreal fps = 1000.0 / (qreal)( t.elapsed() );
    d->paintFps(painter, dirtyRect, fps);
    emit framesPerSecond( fps );
}

void MarbleMap::customPaint(GeoPainter *painter)
{
    Q_UNUSED( painter );

    if ( !viewParams()->mapTheme() ) 
    {
        return;
    }
}

void MarbleMap::goHome()
{
    qreal  homeLon = 0;
    qreal  homeLat = 0;
    d->m_homePoint.geoCoordinates( homeLon, homeLat );

    centerOn( homeLon * RAD2DEG, homeLat * RAD2DEG );

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
    // Quick and dirty way to force a whole update of the view
    d->doResize();
}

void MarbleMap::setShowCrosshairs( bool visible )
{
    setPropertyValue( "crosshairs", visible );
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
    // Update texture map during the repaint that follows:
    setNeedsUpdate();
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
    // Update texture map during the repaint that follows:
    setNeedsUpdate();
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
    setNeedsUpdate();
}

void MarbleMap::setShowFrameRate( bool visible )
{
    d->m_showFrameRate = visible;
}

void MarbleMap::setShowGps( bool visible )
{
    d->m_viewParams.setShowGps( visible );
}

void MarbleMap::changeCurrentPosition( qreal lon, qreal lat)
{
    d->m_model->gpsLayer()->changeCurrentPosition( lat, lon );
}

void MarbleMap::notifyMouseClick( int x, int y)
{
    bool    valid = false;
    qreal  lon   = 0;
    qreal  lat   = 0;

    valid = geoCoordinates( x, y, lon, lat, GeoDataCoordinates::Radian );

    if ( valid ) {
        emit mouseClickGeoPosition( lon, lat, GeoDataCoordinates::Radian);
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

void MarbleMap::clearPersistentTileCache()
{
    d->m_model->clearPersistentTileCache();
}

void MarbleMap::setPersistentTileCacheLimit( quint64 kiloBytes )
{
    d->m_persistentTileCacheLimit = kiloBytes;
    // TODO: trigger update
}

void MarbleMap::clearVolatileTileCache()
{
    d->m_model->clearVolatileTileCache();
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

void MarbleMap::setDownloadUrl( const QUrl &url )
{
    HttpDownloadManager *downloadManager = d->m_model->downloadManager();
    if ( downloadManager != 0 )
        downloadManager->setServerUrl( url );
    else {
        downloadManager = new HttpDownloadManager( url,
                                                   new FileStoragePolicy( MarbleDirs::localPath() ) );
        d->m_model->setDownloadManager( downloadManager );
    }
}

QString MarbleMap::distanceString() const
{
    const qreal VIEW_ANGLE = 110.0;

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
    qreal distance = ( model()->planetRadius() * 0.4
			/ (qreal)( radius() )
			/ tan( 0.5 * VIEW_ANGLE * DEG2RAD ) );

    return QString( "%L1 %2" ).arg( distance, 8, 'f', 1, QChar(' ') ).arg( tr("km") );
}


bool MarbleMap::mapCoversViewport()
{
    return d->m_viewParams.viewport()->mapCoversViewport();
}

Marble::AngleUnit MarbleMap::defaultAngleUnit() const
{
    if ( GeoDataCoordinates::defaultNotation() == GeoDataCoordinates::Decimal )
    {
        return Marble::DecimalDegree;
    }

    return Marble::DMSDegree;
}

void MarbleMap::setDefaultAngleUnit( Marble::AngleUnit angleUnit )
{
    if ( angleUnit == Marble::DecimalDegree )
    {
        GeoDataCoordinates::setDefaultNotation( GeoDataCoordinates::Decimal );
        return;
    }

    GeoDataCoordinates::setDefaultNotation( GeoDataCoordinates::DMS );
}


Marble::DistanceUnit MarbleMap::defaultDistanceUnit() const
{
    return Marble::Metric;
}

void MarbleMap::setDefaultDistanceUnit( Marble::DistanceUnit distanceUnit )
{
}

QFont MarbleMap::defaultFont() const
{
    return GeoDataFeature::defaultFont();
}

void MarbleMap::setDefaultFont( const QFont& font )
{
    GeoDataFeature::setDefaultFont( font );
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

    qreal  lon = sunLocator->getLon();
    qreal  lat = sunLocator->getLat();
    centerOn( lon, lat );

    qDebug() << "Centering on Sun at " << lat << lon;
}

SunLocator* MarbleMap::sunLocator()
{
    return d->m_model->sunLocator();
}

QList<MarbleRenderPlugin *> MarbleMap::renderPlugins() const
{
    return d->m_model->renderPlugins();
}

QList<MarbleAbstractFloatItem *> MarbleMap::floatItems() const
{
    return d->m_model->floatItems();
}

#include "MarbleMap.moc"
