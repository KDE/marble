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
#include <QtCore/QTime>
#include <QtGui/QSizePolicy>
#include <QtGui/QRegion>
#include <QtGui/QStyleOptionGraphicsItem>

//#include <QtDBus/QDBusConnection>

#include "global.h"
#include "AutoSettings.h"
#include "Quaternion.h"
#include "ViewParams.h"
#include "TextureColorizer.h"
#include "ClipPainter.h"
#include "FileViewModel.h"
#include "GeoDataPoint.h"
#include "GpxFileViewItem.h"
#include "MarbleDirs.h"
#include "MarbleWidgetInputHandler.h"
#include "MarbleWidgetPopupMenu.h"
#include "TileCreatorDialog.h"
#include "HttpDownloadManager.h"
#include "FileStoragePolicy.h"
#include "gps/GpsLayer.h"
#include "BoundingBox.h"

#include "MeasureTool.h"


#ifdef Q_CC_MSVC
# ifndef KDEWIN_MATH_H
   static long double sqrt(int a) { return sqrt((long double)a); }
# endif
#endif



class MarbleWidgetPrivate
{
 public:
    // The model we are showing.
    MarbleModel     *m_model;

    ViewParams       m_viewParams;
    bool             m_justModified; // FIXME: Rename to isDirty

    GeoDataPoint     m_homePoint;
    int              m_homeZoom;

    int              m_logzoom;

    int              m_zoomStep;

    MarbleWidgetInputHandler  *m_inputhandler;
    MarbleWidgetPopupMenu     *m_popupmenu;

    TextureColorizer          *m_sealegend;

    // Parameters for the widgets appearance.
    bool             m_showCompass;
    bool             m_showScaleBar;

    bool             m_showFrameRate;

    // Parts of the image in the Widget
    CrossHairFloatItem         m_crosshair;
    CompassFloatItem           m_compass;  // Shown in the upper right
    MapScaleFloatItem          m_mapscale; // Shown in the lower left

    // Tools
    MeasureTool     *m_measureTool;

    QRegion          m_activeRegion;
};



MarbleWidget::MarbleWidget(QWidget *parent)
    : QWidget(parent),
      d( new MarbleWidgetPrivate )
{
//    QDBusConnection::sessionBus().registerObject("/marble", this, QDBusConnection::QDBusConnection::ExportAllSlots);
    d->m_model = new MarbleModel( this );
    construct( parent );
}


MarbleWidget::MarbleWidget(MarbleModel *model, QWidget *parent)
    : QWidget(parent),
      d( new MarbleWidgetPrivate )
{
//    QDBusConnection::sessionBus().registerObject("/marble", this, QDBusConnection::QDBusConnection::ExportAllSlots);
    d->m_model = model;
    construct( parent );
}

MarbleWidget::~MarbleWidget()
{
    // Remove and delete an existing InputHandler
    setInputHandler(NULL);
    setDownloadManager(NULL);
    delete d->m_model;
//    Moved to ViewParams:
//    delete d->m_viewParams.m_canvasImage;
//    delete d->m_viewParams.m_coastImage;
    delete d;
}

void MarbleWidget::construct(QWidget *parent)
{
    setMinimumSize( 200, 300 );
    setFocusPolicy( Qt::WheelFocus );
    setFocus( Qt::OtherFocusReason );

    // Some point that tackat defined. :-)
    setHome( -9.4, 54.8, 1050 );

    connect( d->m_model, SIGNAL( creatingTilesStart( TileCreator*, const QString&, const QString& ) ),
             this,    SLOT( creatingTilesStart( TileCreator*, const QString&, const QString& ) ) );

    connect( d->m_model, SIGNAL(themeChanged( QString )), SIGNAL(themeChanged( QString )) );
    connect( d->m_model, SIGNAL(modelChanged()), this, SLOT(updateChangedMap()) );

    connect( d->m_model, SIGNAL( regionChanged( BoundingBox ) ) ,
             this, SLOT(updateRegion( BoundingBox) ) );


    // Set background: black.
    setPalette( QPalette ( Qt::black ) );

    // Set whether the black space gets displayed or the earth gets simply 
    // displayed on the widget background.
    setAutoFillBackground( true );

    d->m_justModified = false;

    d->m_inputhandler = NULL;
    d->m_popupmenu = new MarbleWidgetPopupMenu( this, d->m_model );
    d->m_measureTool = new MeasureTool( this );

    setInputHandler(new MarbleWidgetDefaultInputHandler);
    setMouseTracking( true );

    connect( d->m_popupmenu,   SIGNAL( addMeasurePoint( double, double ) ),
	     d->m_measureTool, SLOT( addMeasurePoint( double, double ) ) );
    connect( d->m_popupmenu,   SIGNAL( removeMeasurePoints() ),
	     d->m_measureTool, SLOT( removeMeasurePoints( ) ) );

    connect( d->m_model, SIGNAL( timeout() ),
             this,       SLOT( updateGps() ) );


    d->m_logzoom  = 0;
    d->m_zoomStep = 40;

    goHome();

    d->m_showScaleBar  = true;
    d->m_showCompass   = true;
    d->m_showFrameRate = false;

    // Widget translation
    QString      locale = QLocale::system().name();
    QTranslator  translator;
    translator.load(QString("marblewidget_") + locale);
    QCoreApplication::installTranslator(&translator);

#if 0 // Reeneable when the autosettings are actually used

      // AutoSettings
    AutoSettings* autoSettings = new AutoSettings( this );
#endif
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

    if ( d->m_inputhandler )
    {
        d->m_inputhandler->init(this);
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
    d->m_model->setDownloadManager( downloadManager );
}


Quaternion MarbleWidget::planetAxis() const
{
    return d->m_viewParams.m_planetAxis;
}


int MarbleWidget::radius() const
{
    return d->m_viewParams.m_radius;
}

void MarbleWidget::setRadius(const int radius)
{
    d->m_viewParams.m_radius = radius;
}


bool MarbleWidget::needsUpdate() const
{
    return ( d->m_justModified
             || d->m_viewParams.m_radius != d->m_viewParams.m_radiusUpdated
             || !( d->m_viewParams.m_planetAxis == d->m_viewParams.m_planetAxisUpdated ) );
}

void MarbleWidget::setNeedsUpdate()
{
    d->m_justModified = true;
}


QAbstractItemModel *MarbleWidget::placeMarkModel() const
{
    return d->m_model->placeMarkModel();
}

QItemSelectionModel *MarbleWidget::placeMarkSelectionModel() const
{
    return d->m_model->placeMarkSelectionModel();
}

double MarbleWidget::moveStep()
{
    if ( radius() < sqrt( width() * width() + height() * height() ) )
	return 180.0 * 0.1;
    else
	return 180.0 * atan( (double)width()
		     / (double)( 2 * radius() ) ) * 0.2;
}

int MarbleWidget::zoom() const
{
    return d->m_logzoom;
}

double MarbleWidget::centerLatitude() const
{
    // Calculate translation of center point
    double centerLon, centerLat;
    d->m_viewParams.centerCoordinates(centerLon, centerLat);
    return centerLat * RAD2DEG;
}

double MarbleWidget::centerLongitude() const
{
    // Calculate translation of center point
    double centerLon, centerLat;
    d->m_viewParams.centerCoordinates(centerLon, centerLat);
    return centerLon * RAD2DEG;
}

int  MarbleWidget::minimumZoom() const
{
    return d->m_model->minimumZoom();
}

int  MarbleWidget::maximumZoom() const
{
    return d->m_model->maximumZoom();
}

void MarbleWidget::addPlaceMarkFile( const QString &filename )
{
    d->m_model->addPlaceMarkFile( filename );
}

QPixmap MarbleWidget::mapScreenShot()
{
    return QPixmap::grabWidget( this );
}

bool MarbleWidget::showScaleBar() const
{
    return d->m_showScaleBar;
}

bool MarbleWidget::showCompass() const
{
    return d->m_showCompass;
}

bool MarbleWidget::showGrid() const
{
    return d->m_viewParams.m_showGrid;
}

bool MarbleWidget::showPlaces() const
{
    return d->m_viewParams.m_showPlaceMarks;
}

bool MarbleWidget::showCities() const
{
    return d->m_viewParams.m_showCities;
}

bool MarbleWidget::showTerrain() const
{
    return d->m_viewParams.m_showTerrain;
}

bool MarbleWidget::showOtherPlaces() const
{
    return d->m_viewParams.m_showOtherPlaces;
}

bool MarbleWidget::showRelief() const
{
    return d->m_viewParams.m_showRelief;
}

bool MarbleWidget::showElevationModel() const
{
    return d->m_viewParams.m_showElevationModel;
}

bool MarbleWidget::showIceLayer() const
{
    return d->m_viewParams.m_showIceLayer;
}

bool MarbleWidget::showBorders() const
{
    return d->m_viewParams.m_showBorders;
}

bool MarbleWidget::showRivers() const
{
    return d->m_viewParams.m_showRivers;
}

bool MarbleWidget::showLakes() const
{
    return d->m_viewParams.m_showLakes;
}

bool MarbleWidget::showGps() const
{
    return d->m_model->gpsLayer()->visible();
}

bool MarbleWidget::showFrameRate() const
{
    return d->m_showFrameRate;
}

bool  MarbleWidget::quickDirty() const
{
    return d->m_model->textureMapper()->interlaced();
}

void MarbleWidget::zoomView(int newZoom)
{
    // Check for under and overflow.
    if ( newZoom < minimumZoom() ) {
        newZoom = minimumZoom();
    }
    else if ( newZoom > maximumZoom() ) {
        newZoom = maximumZoom();
    }

    // Prevent infinite loops.
    if ( newZoom  == d->m_logzoom )
	return;

    d->m_logzoom = newZoom;

    emit zoomChanged( newZoom );

    int newRadius = fromLogScale( newZoom );

    if ( newRadius == radius() )
	return;

    // Clear canvas if the globe is visible as a whole or if the globe
    // does shrink.
    int  imageHalfWidth  = d->m_viewParams.m_canvasImage->width()  / 2;
    int  imageHalfHeight = d->m_viewParams.m_canvasImage->height() / 2;

    if ( newRadius * newRadius < imageHalfWidth * imageHalfWidth + imageHalfHeight * imageHalfHeight
         && newRadius != radius() 
         || d->m_viewParams.m_projection == Equirectangular )
    {
        setAttribute( Qt::WA_NoSystemBackground, false );
        d->m_viewParams.m_canvasImage->fill( Qt::black );
    }
    else {
        setAttribute( Qt::WA_NoSystemBackground, true );
    }

    setRadius( newRadius );

    emit distanceChanged( distanceString() );

    // We don't do this on every paintEvent to improve performance.
    // Redrawing the atmosphere is only needed if the size of the 
    // globe changes.
    drawAtmosphere();

    repaint();

    setActiveRegion();
}


void MarbleWidget::zoomViewBy( int zoomStep )
{
    zoomView( toLogScale( radius() ) + zoomStep );
}


void MarbleWidget::zoomIn()
{
    zoomViewBy( d->m_zoomStep );
}

void MarbleWidget::zoomOut()
{
    zoomViewBy( -d->m_zoomStep );
}

void MarbleWidget::rotateTo(const Quaternion& quat)
{
    d->m_viewParams.m_planetAxis = quat;

    // This method doesn't force a repaint of the view on purpose!
    // See header file.
}


void MarbleWidget::rotateBy(const Quaternion& incRot)
{
    d->m_viewParams.m_planetAxis = incRot * d->m_viewParams.m_planetAxis;

    repaint();
}

void MarbleWidget::rotateBy( const double& deltaLon, const double& deltaLat)
{
    Quaternion  rotPhi( 1.0, deltaLat / 180.0, 0.0, 0.0 );
    Quaternion  rotTheta( 1.0, 0.0, deltaLon / 180.0, 0.0 );

    d->m_viewParams.m_planetAxis = rotTheta * d->m_viewParams.m_planetAxis;
    d->m_viewParams.m_planetAxis *= rotPhi;
    d->m_viewParams.m_planetAxis.normalize();

    repaint();
}


void MarbleWidget::centerOn(const double& lon, const double& lat)
{
    d->m_viewParams.m_planetAxis.createFromEuler( -lat * DEG2RAD,
                                                   lon * DEG2RAD,
                                                  0.0 );
//    d->m_viewParams.m_planetAxis.display();
    repaint();
}

void MarbleWidget::centerOn(const QModelIndex& index)
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

    repaint();
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
    return d->m_viewParams.m_projection;
}

// FIXME: Actually take a real Projection as parameter.
void MarbleWidget::setProjection( int projectionIndex )
{
    emit projectionChanged( projectionIndex );

    Projection projection;

    switch ( projectionIndex )
    {
        case 0:
            projection = Spherical;
            break;
        case 1:
            projection = Equirectangular;
            break; 
        default: 
            return;
            break;
    }

    d->m_viewParams.m_oldProjection = d->m_viewParams.m_projection;
    d->m_viewParams.m_projection = projection;

    int  imageHalfWidth = d->m_viewParams.m_canvasImage->width() / 2;
    int  imageHalfHeight = d->m_viewParams.m_canvasImage->height() / 2;

    if ( radius() * radius() < imageHalfWidth * imageHalfWidth + imageHalfHeight * imageHalfHeight
         || d->m_viewParams.m_projection == Equirectangular )
    {
        setAttribute(Qt::WA_NoSystemBackground, false);
        d->m_viewParams.m_canvasImage->fill( Qt::black );
    }
    else {
        setAttribute(Qt::WA_NoSystemBackground, true);
    }

    drawAtmosphere();

    // Update texture map during the repaint that follows:
    setMapTheme( d->m_model->mapTheme() );
    setNeedsUpdate();
    repaint();
}

void MarbleWidget::home( double &lon, double &lat, int& zoom )
{
    d->m_homePoint.geoCoordinates( lon, lat, GeoDataPoint::Degree );
    zoom = d->m_homeZoom;
}

void MarbleWidget::setHome( const double lon, const double lat, const int zoom)
{
    d->m_homePoint = GeoDataPoint( lon, lat, 0, GeoDataPoint::Degree );
    d->m_homeZoom = zoom;
}

void MarbleWidget::setHome(const GeoDataPoint& homePoint, int zoom)
{
    d->m_homePoint = homePoint;
    d->m_homeZoom = zoom;
}


void MarbleWidget::moveLeft()
{
    int polarity = 0;

    if ( northPoleY() != 0 )
        polarity = northPoleY() / abs(northPoleY());

    if ( polarity < 0 )
        rotateBy( +moveStep(), 0 );
    else
        rotateBy( -moveStep(), 0 );
}

void MarbleWidget::moveRight()
{
    int polarity = 0;

    if ( northPoleY() != 0 )
        polarity = northPoleY() / abs(northPoleY());

    if ( polarity < 0 )
        rotateBy( -moveStep(), 0 );
    else
        rotateBy( +moveStep(), 0 );
}


void MarbleWidget::moveUp()
{
    rotateBy( 0, -moveStep() );
}

void MarbleWidget::moveDown()
{
    rotateBy( 0, +moveStep() );
}

void MarbleWidget::leaveEvent (QEvent*)
{
    emit mouseMoveGeoPosition( NOT_AVAILABLE );
}

void MarbleWidget::resizeEvent (QResizeEvent*)
{
    //	Redefine the area where the mousepointer becomes a navigationarrow
    setActiveRegion();

    delete d->m_viewParams.m_canvasImage;
    d->m_viewParams.m_canvasImage = new QImage( width(), height(),
                                   QImage::Format_ARGB32_Premultiplied );

    // Clear canvas if the globe is visible as a whole or if the globe
    // does shrink.
    int  imageWidth2  = width() / 2;
    int  imageHeight2 = height() / 2;

    if ( radius() < imageWidth2 * imageWidth2 + imageHeight2 * imageHeight2 ) {
        setAttribute(Qt::WA_NoSystemBackground, false);
        d->m_viewParams.m_canvasImage->fill( Qt::black );
    }
    else {
        setAttribute(Qt::WA_NoSystemBackground, true);
    }

    drawAtmosphere();

    delete d->m_viewParams.m_coastImage;
    d->m_viewParams.m_coastImage = new QImage( width(), height(),
                                               QImage::Format_ARGB32_Premultiplied );
    d->m_justModified = true;
    repaint();
}

void MarbleWidget::connectNotify ( const char * signal )
{
    if ( QByteArray( signal ) == 
         QMetaObject::normalizedSignature ( SIGNAL( mouseMoveGeoPosition( QString ) ) ) )
        if ( d->m_inputhandler ) d->m_inputhandler->setPositionSignalConnected( true );
}

void MarbleWidget::disconnectNotify ( const char * signal )
{
    if ( QByteArray( signal ) == 
         QMetaObject::normalizedSignature ( SIGNAL( mouseMoveGeoPosition( QString ) ) ) )
        if ( d->m_inputhandler ) d->m_inputhandler->setPositionSignalConnected( false );
}

int MarbleWidget::northPoleY()
{
    Quaternion  northPole     = GeoDataPoint( 0.0, M_PI * 0.5 ).quaternion();
    Quaternion  invPlanetAxis = d->m_viewParams.m_planetAxis.inverse();

    northPole.rotateAroundAxis(invPlanetAxis);
    return (int)( d->m_viewParams.m_radius * northPole.v[Q_Y] );
}

int MarbleWidget::northPoleZ()
{
    Quaternion  northPole     = GeoDataPoint( 0.0, M_PI * 0.5 ).quaternion();
    Quaternion  invPlanetAxis = d->m_viewParams.m_planetAxis.inverse();

    northPole.rotateAroundAxis( invPlanetAxis );

    return (int)( d->m_viewParams.m_radius * northPole.v[Q_Z] );
}

bool MarbleWidget::screenCoordinates( const double lon, const double lat,
                                      int& x, int& y )
{
     switch( d->m_viewParams.m_projection ) {
     case Spherical:
     {
         Quaternion p(lon * DEG2RAD, lat * DEG2RAD);
         p.rotateAroundAxis(d->m_viewParams.m_planetAxis.inverse());
 
         x = (int)( width() / 2   + (double)( d->m_viewParams.m_radius ) * p.v[Q_X] );
         y = (int)( height() / 2  + (double)( d->m_viewParams.m_radius ) * p.v[Q_Y] );
 
         return p.v[Q_Z] > 0;
     }
 
     case Equirectangular:
         // Calculate translation of center point
         double centerLon, centerLat;
         d->m_viewParams.centerCoordinates(centerLon, centerLat);
         double rad2Pixel = 2*d->m_viewParams.m_radius / M_PI;
 
         x = (int)( width() / 2 + ( lon * DEG2RAD + centerLon ) * rad2Pixel );
         y = (int)( height() / 2 + ( lat * DEG2RAD + centerLat ) * rad2Pixel );
 
         return true;
     }
 
     return false;
}

bool MarbleWidget::geoCoordinates(const int x, const int y,
                                  double& lon, double& lat,
                                  GeoDataPoint::Unit unit )
{
    int imageHalfWidth  = width() / 2;
    int imageHalfHeight  = height() / 2;
    const double  inverseRadius = 1.0 / (double)(radius());
    bool noerr = false;

    switch( d->m_viewParams.m_projection ) {
    case Spherical:
        if ( radius() > sqrt( ( x - imageHalfWidth ) * ( x - imageHalfWidth )
                                + ( y - imageHalfHeight ) * ( y - imageHalfHeight ) ) )
        {
            double qx = inverseRadius * (double)( x - imageHalfWidth );
            double qy = inverseRadius * (double)( imageHalfHeight - y );
            double qr = 1.0 - qy * qy;

            double qr2z = qr - qx * qx;
            double qz   = ( qr2z > 0.0 ) ? sqrt( qr2z ) : 0.0;

            Quaternion  qpos( 0.0, qx, qy, qz );
            qpos.rotateAroundAxis( planetAxis() );
            qpos.getSpherical( lon, lat );

            noerr = true;
        }
        break;

    case Equirectangular:
        // Calculate translation of center point
        double centerLon, centerLat;
        d->m_viewParams.centerCoordinates(centerLon, centerLat);

        int yCenterOffset =  (int)((double)(2*radius()) / M_PI * centerLat);
        int yTop = imageHalfHeight - radius() + yCenterOffset;
        int yBottom = yTop + 2*radius();
        if ( y >= yTop && y < yBottom ) {
            int const xPixels = x - imageHalfWidth;
            int const yPixels = y - imageHalfHeight;

            double const pixel2rad = M_PI / (2 * radius());
            lat = - yPixels * pixel2rad + centerLat;
            lon = + xPixels * pixel2rad + centerLon;

            while( lon > M_PI ) lon -= 2*M_PI;
            while( lon < -M_PI ) lon += 2*M_PI;

            noerr = true;
        }
        break;
    }

    if ( unit == GeoDataPoint::Degree ) {
        lon *= RAD2DEG;
        lat *= RAD2DEG;
    }

    return noerr;
}

bool MarbleWidget::globalQuaternion( int x, int y, Quaternion &q)
{
    int  imageHalfWidth  = width() / 2;
    int  imageHalfHeight = height() / 2;

    const double  inverseRadius = 1.0 / (double)(radius());

    if ( radius() > sqrt( ( x - imageHalfWidth ) * ( x - imageHalfWidth )
        + ( y - imageHalfHeight ) * ( y - imageHalfHeight ) ) )
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




void MarbleWidget::rotateTo( const double& lon, const double& lat, const double& psi)
{
    d->m_viewParams.m_planetAxis.createFromEuler( -lat * DEG2RAD,   // "phi"
                                                  lon * DEG2RAD,   // "theta"
                                                  psi * DEG2RAD );
}

void MarbleWidget::rotateTo(const double& lon, const double& lat)
{
    d->m_viewParams.m_planetAxis.createFromEuler( -lat * DEG2RAD,
                                                  lon  * DEG2RAD,
                                                  0.0 );
}


void MarbleWidget::drawAtmosphere()
{
    if( d->m_viewParams.m_projection == Spherical &&  4 * radius() * radius() < width() * width() + height() * height()
) {
        int  imageHalfWidth  = width() / 2;
        int  imageHalfHeight = height() / 2;

        // Recalculate the atmosphere effect and paint it to canvasImage.
        QRadialGradient grad1( QPointF( imageHalfWidth, imageHalfHeight ),
                            1.05 * radius() );
        grad1.setColorAt( 0.91, QColor( 255, 255, 255, 255 ) );
        grad1.setColorAt( 1.00, QColor( 255, 255, 255, 0 ) );

        QBrush    brush1( grad1 );
        QPen      pen1( Qt::NoPen );
        QPainter  painter( d->m_viewParams.m_canvasImage );
        painter.setBrush( brush1 );
        painter.setPen( pen1 );
        painter.setRenderHint( QPainter::Antialiasing, false );
        painter.drawEllipse( imageHalfWidth - (int)( (double)(radius()) * 1.05 ),
                            imageHalfHeight - (int)( (double)(radius()) * 1.05 ),
                            (int)( 2.1 * (double)(radius()) ),
                            (int)( 2.1 * (double)(radius()) ) );
    }
}


void MarbleWidget::setActiveRegion()
{
    int zoom = radius();

    d->m_activeRegion = QRegion( 25, 25, width() - 50, height() - 50,
                                 QRegion::Rectangle );

    switch( d->m_viewParams.m_projection ) {
        case Spherical:
            if ( zoom < sqrt( width() * width() + height() * height() ) / 2 ) {
	       d->m_activeRegion &= QRegion( width()  / 2 - zoom, 
                                             height() / 2 - zoom,
                                             2 * zoom, 2 * zoom, 
                                             QRegion::Ellipse );
            }
            break;
        case Equirectangular:
            // Calculate translation of center point
            double centerLon, centerLat;
            d->m_viewParams.centerCoordinates( centerLon, centerLat );

            int yCenterOffset =  (int)((double)(2*zoom) / M_PI * centerLat);
            int yTop = height()/2 - zoom + yCenterOffset;
            d->m_activeRegion &= QRegion( 0, yTop, 
                                          width(), 2 * zoom,
                                          QRegion::Rectangle );
            break;
    }
}

const QRegion MarbleWidget::activeRegion()
{
    return d->m_activeRegion;
}

void MarbleWidget::setBoundingBox()
{
    QVector<QPointF>  points;
    Quaternion        temp;

    if ( globalQuaternion( 0, 0, temp) ) {
        points.append( QPointF( temp.v[Q_X], temp.v[Q_Y]) );
    }
    if ( globalQuaternion( width() / 2, 0, temp ) ) {
        points.append( QPointF( temp.v[Q_X], temp.v[Q_Y]) );
    }

    if ( globalQuaternion( width(), 0, temp ) ) {
        points.append( QPointF( temp.v[Q_X], temp.v[Q_Y]) );
    }
    if ( globalQuaternion( 0, height(), temp ) ) {
        points.append( QPointF( temp.v[Q_X], temp.v[Q_Y]) );
    }

    if ( globalQuaternion( width()/2, height(), temp ) ) {
        points.append( QPointF( temp.v[Q_X], temp.v[Q_Y]) );
    }

    if ( globalQuaternion( width(), height(), temp ) ) {
        points.append( QPointF( temp.v[Q_X], temp.v[Q_Y]) );
    }

    d->m_viewParams.m_boundingBox = BoundingBox( points );
}


void MarbleWidget::paintEvent(QPaintEvent *evt)
{
    QTime t;
    t.start();

    bool doClip = false;
    if( d->m_viewParams.m_projection == Spherical )
        doClip = ( d->m_viewParams.m_radius > d->m_viewParams.m_canvasImage->width() / 2
                     || d->m_viewParams.m_radius > d->m_viewParams.m_canvasImage->height() / 2 );

    // Create a painter that will do the painting.
    ClipPainter painter( this, doClip );

    // 1. Paint the globe itself.
    QRect  dirtyRect = evt->rect();
    d->m_model->paintGlobe( &painter,
                            width(), height(), &d->m_viewParams,
                            needsUpdate()
                            || d->m_viewParams.m_canvasImage->isNull(),
                            dirtyRect );
    d->m_viewParams.m_planetAxisUpdated = d->m_viewParams.m_planetAxis;
    d->m_viewParams.m_radiusUpdated     = d->m_viewParams.m_radius;
    d->m_justModified                   = false;

    customPaint( &painter );

    // 2. Paint the compass
    if ( d->m_showCompass )
        painter.drawPixmap( d->m_viewParams.m_canvasImage->width() - 60, 10,
                            d->m_compass.drawCompassPixmap( d->m_viewParams.m_canvasImage->width(),
                                                            d->m_viewParams.m_canvasImage->height(),
                                                            northPoleY(), d->m_viewParams.m_projection ) );

    // 3. Paint the scale.
    if ( d->m_showScaleBar )
        painter.drawPixmap( 10, d->m_viewParams.m_canvasImage->height() - 40,
                            d->m_mapscale.drawScaleBarPixmap( radius(),
                                                              d->m_viewParams.m_canvasImage-> width() / 2 - 20 ) );

    // 4. Paint the crosshair.
    d->m_crosshair.paint( &painter,
                          d->m_viewParams.m_canvasImage->width(),
                          d->m_viewParams.m_canvasImage->height() );

    // 5. Paint measure points if there are any.
    d->m_measureTool->paintMeasurePoints( &painter, d->m_viewParams, true );

    // Set the region of the image where the user can drag it.
    setActiveRegion();

    //Set the Bounding Box
    setBoundingBox();

    double fps = 1000.0 / (double)( t.elapsed() );

    if ( d->m_showFrameRate == true )
    {
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

void MarbleWidget::customPaint(ClipPainter *painter)
{
    Q_UNUSED( painter );
    /* This is a NOOP */
}

void MarbleWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{
    Q_UNUSED( painter );
    Q_UNUSED( option );
    Q_UNUSED( widget );
}

void MarbleWidget::goHome()
{
    // d->m_model->rotateTo(0, 0);
    double  homeLon = 0;
    double  homeLat = 0;
    d->m_homePoint.geoCoordinates( homeLon, homeLat );

    rotateTo( homeLon * RAD2DEG, homeLat * RAD2DEG );

    zoomView( d->m_homeZoom ); // default 1050

    repaint(); // not obsolete in case the zoomlevel stays unaltered
}

QString MarbleWidget::mapTheme() const
{
    return d->m_model->mapTheme();
}

void MarbleWidget::setMapTheme( const QString& maptheme )
{
    if ( maptheme == d->m_model->mapTheme()
         && d->m_viewParams.m_projection == d->m_viewParams.m_oldProjection )
        return;

    d->m_model->setMapTheme( maptheme, this, d->m_viewParams.m_projection );
    // Update texture map during the repaint that follows:
    setNeedsUpdate();
    repaint();
}

void MarbleWidget::setShowScaleBar( bool visible )
{
    d->m_showScaleBar = visible;
    repaint();
}

void MarbleWidget::setShowCompass( bool visible )
{
    d->m_showCompass = visible;
    repaint();
}

void MarbleWidget::setShowGrid( bool visible )
{
    d->m_viewParams.m_showGrid = visible;
    repaint();
}

void MarbleWidget::setShowPlaces( bool visible )
{
    d->m_viewParams.m_showPlaceMarks = visible;
    repaint();
}

void MarbleWidget::setShowCities( bool visible )
{
    d->m_viewParams.m_showCities = visible;
    repaint();
}

void MarbleWidget::setShowTerrain( bool visible )
{
    d->m_viewParams.m_showTerrain = visible;
    repaint();
}

void MarbleWidget::setShowOtherPlaces( bool visible )
{
    d->m_viewParams.m_showOtherPlaces = visible;
    repaint();
}

void MarbleWidget::setShowRelief( bool visible )
{
    d->m_viewParams.m_showRelief = visible;
    // Update texture map during the repaint that follows:
    setNeedsUpdate();
    repaint();
}

void MarbleWidget::setShowElevationModel( bool visible )
{
    d->m_viewParams.m_showElevationModel = visible;
    // Update texture map during the repaint that follows:
    setNeedsUpdate();
    repaint();
}

void MarbleWidget::setShowIceLayer( bool visible )
{
    d->m_viewParams.m_showIceLayer = visible;
    // Update texture map during the repaint that follows:
    setNeedsUpdate();
    repaint();
}

void MarbleWidget::setShowBorders( bool visible )
{
    d->m_viewParams.m_showBorders = visible;
    repaint();
}

void MarbleWidget::setShowRivers( bool visible )
{
    d->m_viewParams.m_showRivers =  visible;
    repaint();
}

void MarbleWidget::setShowLakes( bool visible )
{
    d->m_viewParams.m_showLakes = visible;
    // Update texture map during the repaint that follows:
    setNeedsUpdate();
    repaint();
}

void MarbleWidget::setShowFrameRate( bool visible )
{
    d->m_showFrameRate = visible;
    repaint();
}

void MarbleWidget::setShowGps( bool visible )
{
    d->m_viewParams.m_showGps = visible;
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
    draw = d->m_model->gpsLayer()->updateGps( size(),&d->m_viewParams, temp );
    if( draw ){
        update(temp);
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

void MarbleWidget::setQuickDirty( bool enabled )
{
    int transparency;
    switch( d->m_viewParams.m_projection ) {
        case Spherical:
            // Interlace texture mapping
            d->m_model->textureMapper()->setInterlaced( enabled );
            // Update texture map during the repaint that follows:
            setNeedsUpdate();
            transparency = enabled ? 255 : 192;
            d->m_compass.setTransparency( transparency );
            d->m_mapscale.setTransparency( transparency );
            repaint();
            break;
        case Equirectangular:
            return;
    }
}

// This slot will called when the Globe starts to create the tiles.

void MarbleWidget::creatingTilesStart( TileCreator *creator, const QString &name, const QString &description )
{
    qDebug("MarbleWidget::creatingTilesStart called... ");

    TileCreatorDialog dlg( creator, this );
    dlg.setSummary( name, description );
    dlg.exec();

    qDebug("MarbleWidget::creatingTilesStart exits... ");
}


void MarbleWidget::updateChangedMap()
{
    // Update texture map during the repaint that follows:
    setNeedsUpdate();
    update();
}

void MarbleWidget::updateRegion( BoundingBox box )
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

void MarbleWidget::setDownloadUrl( const QUrl &url ) {
    HttpDownloadManager *downloadManager = d->m_model->downloadManager();
    if ( downloadManager != NULL )
        downloadManager->setServerUrl( url );
    else
    {
        downloadManager = new HttpDownloadManager( url,
                                                   new FileStoragePolicy( MarbleDirs::localPath() ) );
        d->m_model->setDownloadManager( downloadManager );
    }
}

int MarbleWidget::fromLogScale(int zoom)
{
    zoom = (int) pow( M_E, ( (double)zoom / 200.0 ) );
    // zoom = (int) pow(2.0, ((double)zoom/200));
    return zoom;
}

int MarbleWidget::toLogScale(int zoom)
{
    zoom = (int)(200.0 * log( (double)zoom ) );
    return zoom;
}

QString MarbleWidget::distanceString() const
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

#include "MarbleWidget.moc"
