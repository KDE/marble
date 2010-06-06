//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2009 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Carlos Licea <carlos.licea@kdemail.net>
// Copyright 2009      Jens-Michael Hoffmann <jensmh@gmx.de>
//


// Own
#include "MarbleMap.h"
#include "MarbleMap_p.h"

// Posix
#include <cmath>

// Qt
#include <QtCore/QAbstractItemModel>
#include <QtCore/QTime>
#include <QtGui/QItemSelectionModel>
#include <QtGui/QSizePolicy>
#include <QtGui/QRegion>

#ifdef MARBLE_DBUS
#include <QtDBus/QDBusConnection>
#endif

// Marble
#include "AbstractProjection.h"
#include "AbstractScanlineTextureMapper.h"
#include "FileStorageWatcher.h"
#include "FileViewModel.h"
#include "GeoDataFeature.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoPainter.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneZoom.h"
#include "HttpDownloadManager.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleLocale.h"
#include "MarbleModel.h"
#include "MarblePlacemarkModel.h"
#include "MeasureTool.h"
#include "MergedLayerDecorator.h"
#include "PlacemarkLayout.h"
#include "Planet.h"
#include "RenderPlugin.h"
#include "StoragePolicy.h"
#include "SunLocator.h"
#include "TextureColorizer.h"
#include "ViewParams.h"
#include "ViewportParams.h"
#include "gps/GpsLayer.h"
#include "gps/PositionTracking.h"

using namespace Marble;

#ifdef Q_CC_MSVC
# ifndef KDEWIN_MATH_H
   static long double sqrt( int a ) { return sqrt( (long double)a ); }
# endif
#endif


MarbleMapPrivate::MarbleMapPrivate( MarbleMap *parent )
        : m_parent( parent ),
          m_persistentTileCacheLimit( 0 ), // No limit
          m_volatileTileCacheLimit( 1024*1024*30 ), // 30 MB
          m_viewAngle( 110.0 )
{
}

void MarbleMapPrivate::construct()
{
    // Some point that tackat defined. :-)
    m_parent->setHome( -9.4, 54.8, 1050 );

    m_parent->connect( m_model, SIGNAL( themeChanged( QString ) ),
                       m_parent, SIGNAL( themeChanged( QString ) ) );
    m_parent->connect( m_model, SIGNAL( modelChanged() ),
                       m_parent, SLOT( updateChangedMap() ) );

    m_justModified = false;
    m_dirtyAtmosphere = false;
    
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

    m_parent->connect( m_model,               SIGNAL( repaintNeeded( QRegion ) ),
                       m_parent,              SIGNAL( repaintNeeded( QRegion ) ) );

    // A new instance of FileStorageWatcher.
    // The thread will be started at setting persistent tile cache size.
    m_storageWatcher = new FileStorageWatcher( MarbleDirs::localPath(), m_parent );
    m_parent->connect( m_parent, SIGNAL( themeChanged( QString ) ),
		       m_storageWatcher, SLOT( updateTheme( QString ) ) );
    // Setting the theme to the current theme.
    m_storageWatcher->updateTheme( m_parent->mapThemeId() );
    // connect the StoragePolicy used by the download manager to the FileStorageWatcher
    StoragePolicy * const storagePolicy = m_model->downloadManager()->storagePolicy();
    QObject::connect( storagePolicy, SIGNAL( cleared() ),
                      m_storageWatcher, SLOT( resetCurrentSize() ) );
    QObject::connect( storagePolicy, SIGNAL( sizeChanged( qint64 ) ),
                      m_storageWatcher, SLOT( addToCurrentSize( qint64 ) ) );
}

// Used to be resizeEvent()
void MarbleMapPrivate::doResize()
{
    QSize size( m_parent->width(), m_parent->height() );
    m_viewParams.viewport()->setSize( size );

    // If the globe covers fully the screen then we can use the faster
    // RGB32 as there are no translucent areas involved.
    QImage::Format imageFormat = ( m_parent->mapCoversViewport() )
                                 ? QImage::Format_RGB32
                                 : QImage::Format_ARGB32_Premultiplied;

    // Recreate the canvas image with the new size.
    m_viewParams.setCanvasImage( new QImage( m_parent->width(), m_parent->height(),
                                             imageFormat ));

    if ( m_viewParams.showAtmosphere() ) {
        m_dirtyAtmosphere=true;
    }

    // Recreate the coastline detection offscreen image
    m_viewParams.setCoastImage( new QImage( m_parent->width(), m_parent->height(),
                                            QImage::Format_RGB32 ));

    m_justModified = true;
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

void MarbleMapPrivate::paintGround( GeoPainter &painter, QRect &dirtyRect )
{
    if ( !m_viewParams.mapTheme() ) {
        mDebug() << "No theme yet!";
        paintMarbleSplash( painter, dirtyRect );
        return;
    }

    if ( m_dirtyAtmosphere ) {
        drawAtmosphere();
        m_dirtyAtmosphere = false;
    }

    m_model->paintGlobe( &painter,
                         m_parent->width(), m_parent->height(), &m_viewParams,
                         m_parent->needsUpdate() || m_viewParams.canvasImage()->isNull(),
                         dirtyRect );
    // FIXME: this is ugly, add method updatePlanetAxis() to ViewParams
    m_viewParams.setPlanetAxisUpdated( m_viewParams.planetAxis() );
    // FIXME: this is ugly, add method updateRadius() to ViewParams
    m_viewParams.setRadiusUpdated( m_viewParams.radius() );
    m_justModified = false;

    // FIXME: This is really slow. That's why we defer this to
    //        PrintQuality. Either cache on a pixmap - or maybe
    //        better: Add to GlobeScanlineTextureMapper.

    if ( m_viewParams.mapQuality() == PrintQuality )
        drawFog( painter );
}

void MarbleMapPrivate::paintOverlay( GeoPainter &painter, QRect &dirtyRect )
{
    Q_UNUSED( dirtyRect )

    if ( !m_viewParams.mapTheme() ) {
        return;
    }

    // FIXME: Add this stuff into the Layermanager as something to be 
    // called before the float items.

    bool antialiased = false;

    if (   m_viewParams.mapQuality() == HighQuality
        || m_viewParams.mapQuality() == PrintQuality ) {
            antialiased = true;
    }

    m_measureTool->paint( &painter, m_viewParams.viewport(), antialiased );
}

void MarbleMapPrivate::paintFps( GeoPainter &painter, QRect &dirtyRect, qreal fps )
{
    Q_UNUSED( dirtyRect );

    if ( m_showFrameRate ) {
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
#ifdef MARBLE_DBUS
    QDBusConnection::sessionBus().registerObject( "/MarbleMap", this, 
                                                  QDBusConnection::ExportAllSlots
                                                  | QDBusConnection::ExportAllSignals
                                                  | QDBusConnection::ExportAllProperties );
#endif
    QTime t;
    t.start();
    
    d->m_model        = new MarbleModel( this );
    d->m_modelIsOwned = true;

    d->construct();
    d->m_model->setupVectorComposer();
    qDebug("Model: Time elapsed: %d ms", t.elapsed());
}

MarbleMap::MarbleMap(MarbleModel *model)
    : d( new MarbleMapPrivate( this ) )
{
//     QDBusConnection::sessionBus().registerObject( "/marble", this,
//                                                   QDBusConnection::ExportAllSlots );

    d->m_model        = model;
    d->m_modelIsOwned = false;

    d->construct();
    d->m_model->setupVectorComposer();
}

MarbleMap::~MarbleMap()
{
    // Some basic initializations.
    d->m_width  = 0;
    d->m_height = 0;

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


void MarbleMap::setSize( int width, int height )
{
    d->m_width  = width;
    d->m_height = height;

    d->doResize();
    emit visibleLatLonAltBoxChanged( d->m_viewParams.viewport()->viewLatLonAltBox() );
}

void MarbleMap::setSize( const QSize& size )
{
    d->m_width  = size.width();
    d->m_height = size.height();

    d->doResize();
    emit visibleLatLonAltBoxChanged( d->m_viewParams.viewport()->viewLatLonAltBox() );
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

    if ( !mapCoversViewport() ) {
        setNeedsUpdate();
    }

    d->m_logzoom = d->zoom( radius );
    emit zoomChanged( d->m_logzoom );
    emit distanceChanged( distanceString() );
    emit visibleLatLonAltBoxChanged( d->m_viewParams.viewport()->viewLatLonAltBox() );
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


QAbstractItemModel *MarbleMap::placemarkModel() const
{
    return d->m_model->placemarkModel();
}

QItemSelectionModel *MarbleMap::placemarkSelectionModel() const
{
    return d->m_model->placemarkSelectionModel();
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
    return distanceFromRadius(radius());
}

qreal MarbleMap::distanceFromRadius( qreal radius ) const
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

qreal MarbleMap::radiusFromDistance( qreal distance ) const
{      
    return  model()->planet()->radius() /
            ( distance * tan( 0.5 * d->m_viewAngle * DEG2RAD ) / 0.4 );
}

void MarbleMap::setDistance( qreal newDistance )
{
    qreal minDistance = 0.001;

    if ( newDistance <= minDistance ) {
        mDebug() << "Invalid distance: 0 m";
        newDistance = minDistance;
    }    

    int newRadius = radiusFromDistance( newDistance );
    setRadius( newRadius );
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

void MarbleMap::addPlacemarkFile( const QString &filename )
{
    d->m_model->addPlacemarkFile( filename );
}

void MarbleMap::addPlacemarkData( const QString &data, const QString &key )
{
    d->m_model->addPlacemarkData( data, key );
}

void MarbleMap::removePlacemarkKey( const QString &key )
{
    d->m_model->removePlacemarkKey( key );
}


QPixmap MarbleMap::mapScreenShot()
{
    QPixmap screenshotPixmap( size() );
    screenshotPixmap.fill( Qt::transparent );

    GeoPainter painter( &screenshotPixmap, viewParams()->viewport(),
                        PrintQuality );
    painter.begin( &screenshotPixmap );
    QRect dirtyRect( QPoint(), size() );
    paint( painter, dirtyRect );
    painter.end();

    return screenshotPixmap;
}

void MarbleMap::reload() const
{
    d->m_model->reloadMap();
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

void MarbleMap::zoomView( int newZoom )
{
    // Check for under and overflow.
    if ( newZoom < minimumZoom() )
        newZoom = minimumZoom();
    else if ( newZoom > maximumZoom() )
        newZoom = maximumZoom();

    // Prevent infinite loops.
    if ( newZoom  == d->m_logzoom )
        return;
    setRadius( d->radius( newZoom ) );

    // We don't do this on every paintEvent to improve performance.
    // Redrawing the atmosphere is only needed if the size of the
    // globe changes.
    if ( d->m_viewParams.showAtmosphere() ) {
        d->m_dirtyAtmosphere=true;
    }
}


void MarbleMap::zoomViewBy( int zoomStep )
{
    zoomView( zoom() + zoomStep );
}


void MarbleMap::zoomIn()
{
    zoomViewBy( d->m_zoomStep );
}

void MarbleMap::zoomOut()
{
    zoomViewBy( -d->m_zoomStep );
}

void MarbleMap::rotateBy( const Quaternion& incRot )
{
    d->m_viewParams.setPlanetAxis( incRot * d->m_viewParams.planetAxis() );
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
    emit visibleLatLonAltBoxChanged( d->m_viewParams.viewport()->viewLatLonAltBox() );
}


void MarbleMap::centerOn( const qreal lon, const qreal lat )
{
    Quaternion  quat;
    quat.createFromEuler( -lat * DEG2RAD, lon * DEG2RAD, 0.0 );
    d->m_viewParams.setPlanetAxis( quat );
    emit visibleLatLonAltBoxChanged( d->m_viewParams.viewport()->viewLatLonAltBox() );
}

void MarbleMap::centerOn( const QModelIndex& index )
{
    QItemSelectionModel *selectionModel = d->m_model->placemarkSelectionModel();
    Q_ASSERT( selectionModel );

    selectionModel->clear();

    if ( index.isValid() ) {
        const GeoDataCoordinates point =
            index.data( MarblePlacemarkModel::CoordinateRole ).value<GeoDataCoordinates>();
  
        qreal  lon;
        qreal  lat;
        point.geoCoordinates( lon, lat );

        centerOn( lon * RAD2DEG, lat * RAD2DEG );

        selectionModel->select( index, QItemSelectionModel::SelectCurrent );
    }
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
        d->m_dirtyAtmosphere=true;
    }

    d->m_model->setupTextureMapper( projection );

    // Update texture map during the repaint that follows:
    setNeedsUpdate();
    emit visibleLatLonAltBoxChanged( d->m_viewParams.viewport()->viewLatLonAltBox() );
}

void MarbleMap::home( qreal &lon, qreal &lat, int& zoom )
{
    d->m_homePoint.geoCoordinates( lon, lat, GeoDataCoordinates::Degree );
    zoom = d->m_homeZoom;
}

void MarbleMap::setHome( qreal lon, qreal lat, int zoom )
{
    d->m_homePoint = GeoDataCoordinates( lon, lat, 0, GeoDataCoordinates::Degree );
    d->m_homeZoom = zoom;
}

void MarbleMap::setHome( const GeoDataCoordinates& homePoint, int zoom )
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
                                   qreal& x, qreal& y )
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

// Used to be paintEvent()
void MarbleMap::paint( GeoPainter &painter, QRect &dirtyRect )
{
    QTime t;
    t.start();
    
    d->paintGround( painter, dirtyRect );
    customPaint( &painter );
    d->paintOverlay( painter, dirtyRect );

    qreal fps = 1000.0 / (qreal)( t.elapsed() );
    d->paintFps( painter, dirtyRect, fps );
    emit framesPerSecond( fps );
}

void MarbleMap::customPaint( GeoPainter *painter )
{
    Q_UNUSED( painter );

    if ( !viewParams()->mapTheme() ) {
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

    if ( mapTheme ) {
        d->m_model->setMapTheme( mapTheme, d->m_viewParams.projection() );

        // We don't do this on every paintEvent to improve performance.
        // Redrawing the atmosphere is only needed if the size of the
        // globe changes.
        d->doResize();
        d->m_dirtyAtmosphere=true;

        centerSun();

        // Update texture map during the repaint that follows:
        setNeedsUpdate();
    }
}

void MarbleMap::setPropertyValue( const QString& name, bool value )
{
    mDebug() << "In MarbleMap the property " << name << "was set to " << value;
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
    bool previousVisible = showAtmosphere();

    if ( visible != previousVisible ) {
        d->m_viewParams.setShowAtmosphere( visible );

        // Quick and dirty way to force a whole update of the view
        d->doResize();
    }
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
    setNeedsUpdate();
}

void MarbleMap::setShowTileId( bool visible )
{
    d->m_model->layerDecorator()->setShowTileId( visible );
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

void MarbleMap::changeCurrentPosition( qreal lon, qreal lat )
{
    d->m_model->gpsLayer()->changeCurrentPosition( lat, lon );
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

void MarbleMap::updateGps()
{
    QRegion temp;
//    const bool draw = 
    d->m_model->positionTracking()->update( size(),&d->m_viewParams, temp );
#if 0  // FIXME: move to MarbleWidget?
    if ( draw ) {
        update(temp);
    }
#endif
}

void MarbleMap::openGpxFile( const QString &filename )
{
    d->m_model->openGpxFile( filename );
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
    d->m_storageWatcher->setCacheLimit( kiloBytes * 1024 );
    
    if( kiloBytes != 0 )
    {
	if( !d->m_storageWatcher->isRunning() )
	    d->m_storageWatcher->start( QThread::IdlePriority );
    }
    else
    {
	d->m_storageWatcher->quit();
    }
    // TODO: trigger update
}

void MarbleMap::clearVolatileTileCache()
{
    d->m_model->clearVolatileTileCache();
}

void MarbleMap::setVolatileTileCacheLimit( quint64 kilobytes )
{
    mDebug() << "kiloBytes" << kilobytes;
    d->m_volatileTileCacheLimit = kilobytes;
    d->m_model->setVolatileTileCacheLimit( kilobytes );
}

void MarbleMap::updateChangedMap()
{
    // Update texture map during the repaint that follows:
    setNeedsUpdate();
}

QString MarbleMap::distanceString() const
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
    d->m_model->placemarkLayout()->requestStyleReset();
}

void MarbleMap::updateSun()
{
    // Update the sun shading.
    //SunLocator  *sunLocator = d->m_model->sunLocator();

    mDebug() << "MarbleMap: Updating the sun shading map...";
    d->m_model->update();
    setNeedsUpdate();
    //mDebug() << "Finished updating the sun shading map";
}

void MarbleMap::centerSun()
{
    SunLocator  *sunLocator = d->m_model->sunLocator();

    if ( sunLocator && sunLocator->getCentered() ) {
        qreal  lon = sunLocator->getLon();
        qreal  lat = sunLocator->getLat();
        centerOn( lon, lat );

        mDebug() << "Centering on Sun at " << lat << lon;
    }
}

SunLocator* MarbleMap::sunLocator()
{
    return d->m_model->sunLocator();
}

QList<RenderPlugin *> MarbleMap::renderPlugins() const
{
    return d->m_model->renderPlugins();
}

QList<AbstractFloatItem *> MarbleMap::floatItems() const
{
    return d->m_model->floatItems();
}

void MarbleMap::flyTo( const GeoDataLookAt &lookAt )
{
    int zoom = zoomFromDistance( lookAt.range() * METER2KM );
    if ( zoom < minimumZoom() || zoom > maximumZoom() )
        return; // avoid moving when zooming is impossible

    setDistance( lookAt.range() * METER2KM );
    GeoDataCoordinates::Unit deg = GeoDataCoordinates::Degree;
    centerOn( lookAt.longitude( deg ), lookAt.latitude( deg ) );
}

GeoDataLookAt MarbleMap::lookAt() const
{
    GeoDataLookAt result;
    qreal lon( 0.0 ), lat( 0.0 );

    d->m_viewParams.centerCoordinates( lon, lat );
    result.setLongitude( lon );
    result.setLatitude( lat );
    result.setAltitude( 0.0 );
    result.setRange( distance() * KM2METER );

    return result;
}

qreal MarbleMap::distanceFromZoom( qreal zoom ) const
{
    return distanceFromRadius( d->radius( zoom ) );
}

qreal MarbleMap::zoomFromDistance( qreal distance ) const
{
    return d->zoom( radiusFromDistance( distance ) );
}


#include "MarbleMap.moc"
