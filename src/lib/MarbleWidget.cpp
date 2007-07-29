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

#include "Quaternion.h"
#include "ViewParams.h"
#include "texcolorizer.h"
#include "ClipPainter.h"
#include "MarbleWidgetInputHandler.h"
#include "MarbleWidgetPopupMenu.h"
#include "katlastilecreatordialog.h"
#include "gps/GpsLayer.h"
#include "utils.h"

#include "measuretool.h"


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

    GeoPoint         m_homePoint;
    int              m_homeZoom;

    int              m_logzoom;
	
    int              m_zoomStep;
    int              m_minimumzoom;    
    int              m_maximumzoom;

    MarbleWidgetInputHandler  *m_inputhandler;
    MarbleWidgetPopupMenu     *m_popupmenu;

    TextureColorizer        *m_sealegend;

    // Parameters for the widgets appearance.
    bool             m_showScaleBar;
    bool             m_showWindRose;

    // Parts of the image in the Widget
    KAtlasCrossHair  m_crosshair;
    KAtlasMapScale   m_mapscale; // Shown in the lower left
    KAtlasWindRose   m_windrose; // Shown in the upper right

    // Tools
    MeasureTool     *m_measureTool;

    QRegion          m_activeRegion;

    // The progress dialog for the tile creator.
    KAtlasTileCreatorDialog  *m_tileCreatorDlg;

};



MarbleWidget::MarbleWidget(QWidget *parent)
    : QWidget(parent),
      d( new MarbleWidgetPrivate )
{
    d->m_model = new MarbleModel( this );
    construct( parent );
}


MarbleWidget::MarbleWidget(MarbleModel *model, QWidget *parent)
    : QWidget(parent),
      d( new MarbleWidgetPrivate )
{
    d->m_model = model;
    construct( parent );
}

MarbleWidget::~MarbleWidget()
{
    delete d->m_model;
}

void MarbleWidget::construct(QWidget *parent)
{
    setMinimumSize( 200, 300 );
    setFocusPolicy( Qt::WheelFocus );
    setFocus( Qt::OtherFocusReason );

    // Some point that tackat defined. :-) 
    setHome( 54.8, -9.4, 1050 );

    connect( d->m_model, SIGNAL( creatingTilesStart( const QString&, const QString& ) ),
             this,    SLOT( creatingTilesStart( const QString&, const QString& ) ) );
    connect( d->m_model, SIGNAL( creatingTilesProgress( int ) ),
             this,    SLOT( creatingTilesProgress( int ) ) );

    connect( d->m_model, SIGNAL(modelChanged()), this, SLOT(updateChangedMap()) );

    // Set background: black.
    QPalette p = palette();
    p.setColor( QPalette::Window, Qt::black );
    setPalette( p );
    setBackgroundRole( QPalette::Window );
    setAutoFillBackground( true );

//    setAttribute(Qt::WA_NoSystemBackground);

    d->m_viewParams.m_canvasImage = new QImage( parent->width(), 
                                                parent->height(),
                                         QImage::Format_ARGB32_Premultiplied );
    d->m_justModified = false;


    d->m_inputhandler = new MarbleWidgetInputHandler( this, d->m_model );
    installEventFilter( d->m_inputhandler );
    setMouseTracking( true );

    d->m_popupmenu = new MarbleWidgetPopupMenu( this, d->m_model );
    connect( d->m_inputhandler, SIGNAL( lmbRequest( int, int ) ),
	     d->m_popupmenu,    SLOT( showLmbMenu( int, int ) ) );	
    connect( d->m_inputhandler, SIGNAL( rmbRequest( int, int ) ),
	     d->m_popupmenu,    SLOT( showRmbMenu( int, int ) ) );	
    connect( d->m_inputhandler, SIGNAL( gpsCoordinates( int, int) ),
             this, SLOT( gpsCoordinatesClick( int, int ) ) );

    connect( d->m_inputhandler, SIGNAL( mouseGeoPosition( QString ) ),
         this, SIGNAL( mouseGeoPosition( QString ) ) ); 

    d->m_measureTool = new MeasureTool( this );

    connect( d->m_popupmenu,   SIGNAL( addMeasurePoint( double, double ) ),
	     d->m_measureTool, SLOT( addMeasurePoint( double, double ) ) );
    connect( d->m_popupmenu,   SIGNAL( removeMeasurePoints() ),
	     d->m_measureTool, SLOT( removeMeasurePoints( ) ) );	
    
    connect( d->m_model, SIGNAL( timeout() ),
             this, SLOT( updateGps() ) );

    d->m_logzoom  = 0;
    d->m_zoomStep = 40;
    goHome();

    d->m_minimumzoom = 950;
    d->m_maximumzoom = 2200;

    d->m_showScaleBar = true;
    d->m_showWindRose = true;

    QString locale = QLocale::system().name();
    QTranslator translator;
    translator.load(QString("marblewidget_") + locale);
    QCoreApplication::installTranslator(&translator);
}

MarbleModel *MarbleWidget::model() const
{
    return d->m_model;
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


QAbstractListModel *MarbleWidget::placeMarkModel()
{
    return d->m_model->getPlaceMarkModel();
}

double MarbleWidget::moveStep()
{
    if ( radius() < sqrt( width() * width() + height() * height() ) )
	return 0.1;
    else
	return atan( (double)width() 
		     / (double)( 2 * radius() ) ) * 0.2;
}

int MarbleWidget::zoom() const
{
    return d->m_logzoom; 
}

double MarbleWidget::centerLatitude() const
{
    return d->m_viewParams.m_planetAxis.pitch() * 180.0 / M_PI;
}

double MarbleWidget::centerLongitude() const
{
    return - d->m_viewParams.m_planetAxis.yaw() * 180.0 / M_PI;
}

void MarbleWidget::setMinimumZoom( int zoom )
{
    d->m_minimumzoom = zoom; 
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

bool MarbleWidget::showWindRose() const
{ 
    return d->m_showWindRose;
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

bool MarbleWidget::showRelief() const
{ 
    return d->m_model->textureColorizer()->showRelief();
}

bool MarbleWidget::showElevationModel() const
{ 
    return d->m_viewParams.m_showElevationModel;
}

bool MarbleWidget::showIceLayer() const
{ 
    return d->m_model->vectorComposer()->showIceLayer();
}

bool MarbleWidget::showBorders() const
{ 
    return d->m_model->vectorComposer()->showBorders();
}

bool MarbleWidget::showRivers() const
{ 
    return d->m_model->vectorComposer()->showRivers();
}

bool MarbleWidget::showLakes() const
{ 
    return d->m_model->vectorComposer()->showLakes();
}

bool MarbleWidget::showGps() const
{
    return d->m_model->gpsLayer()->visible();
}

bool  MarbleWidget::quickDirty() const
{ 
#ifndef FLAT_PROJ
    return d->m_model->textureMapper()->interlaced();
#else
    return false;
#endif
}


void MarbleWidget::zoomView(int zoom)
{
    // Prevent infinite loops.
    if ( zoom  == d->m_logzoom )
	return;

    d->m_logzoom = zoom;

    emit zoomChanged(zoom);

    int newRadius = fromLogScale(zoom);

    if ( newRadius == radius() )
	return;
 
    // Clear canvas if the globe is visible as a whole or if the globe
    // does shrink.
    int  imgrx = d->m_viewParams.m_canvasImage->width() / 2;
    int  imgry = d->m_viewParams.m_canvasImage->height() / 2;

    if ( newRadius * newRadius < imgrx * imgrx + imgry * imgry
         && newRadius != radius() )
    {
        setAttribute(Qt::WA_NoSystemBackground, false);
        d->m_viewParams.m_canvasImage->fill( Qt::transparent );
    }
    else
    {
        setAttribute(Qt::WA_NoSystemBackground, true);
    }

    setRadius( newRadius );
    drawAtmosphere();

    repaint();

    setActiveRegion();
}


void MarbleWidget::zoomViewBy(int zoomStep)
{
    // Prevent infinite loops

    int zoom    = radius();
    int tryZoom = toLogScale(zoom) + zoomStep;
    // qDebug() << QString::number(tryZoom) << " "
    //         << QString::number(minimumzoom);
    if ( tryZoom >= d->m_minimumzoom && tryZoom <= d->m_maximumzoom ) {
	zoom = tryZoom;
	zoomView(zoom);
    }
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

    repaint();
}


void MarbleWidget::rotateBy(const Quaternion& incRot)
{
    d->m_viewParams.m_planetAxis = incRot * d->m_viewParams.m_planetAxis;

    repaint();
}

void MarbleWidget::rotateBy(const double& phi, const double& theta)
{
    Quaternion  rotPhi( 1.0, phi, 0.0, 0.0 );
    Quaternion  rotTheta( 1.0, 0.0, theta, 0.0 );

    d->m_viewParams.m_planetAxis = rotTheta * d->m_viewParams.m_planetAxis;
    d->m_viewParams.m_planetAxis *= rotPhi;
    d->m_viewParams.m_planetAxis.normalize();

    repaint();
}


void MarbleWidget::centerOn(const double& lat, const double& lon)
{
    d->m_viewParams.m_planetAxis.createFromEuler( (lat + 180.0) * M_PI / 180.0,
                                                  (lon + 180.0) * M_PI / 180.0,
                                                  0.0 );

    repaint();
}

void MarbleWidget::centerOn(const QModelIndex& index)
{

    PlaceMarkModel* model = (PlaceMarkModel*) d->m_model->getPlaceMarkModel();
    if (model == 0) qDebug( "model null" );

    PlaceMark* mark = model->placeMark( index );

    d->m_model->placeMarkContainer()->clearSelected();

    if ( mark != 0 ){
	double  lon;
        double  lat;

	mark->coordinate( lon, lat );
	centerOn( -lat * 180.0 / M_PI, -lon * 180.0 / M_PI );
	mark->setSelected( 1 );
	d->m_crosshair.setEnabled( true );
    }
    else 
	d->m_crosshair.setEnabled( false );

    d->m_model->placeMarkContainer()->clearTextPixmaps();
    d->m_model->placeMarkContainer()->sort();

    repaint();
}


void MarbleWidget::setCenterLatitude( double lat )
{ 
    centerOn( lat, centerLongitude() );
}

void MarbleWidget::setCenterLongitude( double lng )
{
    centerOn( centerLatitude(), lng );
}


void MarbleWidget::setHome( const double &lon, const double &lat, int zoom)
{
    d->m_homePoint = GeoPoint( lon, lat );
    d->m_homeZoom = zoom;
}

void MarbleWidget::setHome(const GeoPoint& _homePoint, int zoom)
{
    d->m_homePoint = _homePoint;
    d->m_homeZoom = zoom;
}


void MarbleWidget::moveLeft()
{
    int polarity = 0;

    if ( northPoleY() != 0 ) 
        polarity = northPoleY() / abs(northPoleY());

    if ( polarity < 0 )
        rotateBy( 0, +moveStep() );
    else
        rotateBy( 0, -moveStep() );
}

void MarbleWidget::moveRight()
{
    int polarity = 0;

    if ( northPoleY() != 0 ) 
        polarity = northPoleY() / abs(northPoleY());

    if ( polarity < 0 )
        rotateBy( 0, -moveStep() );
    else
        rotateBy( 0, +moveStep() );
}


void MarbleWidget::moveUp()
{
    rotateBy( moveStep(), 0 );
}

void MarbleWidget::moveDown()
{
    rotateBy( -moveStep(), 0 );
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
    int  imgrx = d->m_viewParams.m_canvasImage->width() / 2;
    int  imgry = d->m_viewParams.m_canvasImage->height() / 2;

    if ( radius() < imgrx * imgrx + imgry * imgry )
    {
        setAttribute(Qt::WA_NoSystemBackground, false);
        d->m_viewParams.m_canvasImage->fill( Qt::transparent );
    }
    else
    {
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
    if (QLatin1String(signal) == SIGNAL(mouseGeoPosition(QString)))
        d->m_inputhandler->setPositionSignalConnected(true);
}

void MarbleWidget::disconnectNotify ( const char * signal )
{
    if (QLatin1String(signal) == SIGNAL(mouseGeoPosition(QString)))
        d->m_inputhandler->setPositionSignalConnected(false);
}

int MarbleWidget::northPoleY()
{
    Quaternion  northPole     = GeoPoint( 0.0, -M_PI * 0.5 ).quaternion();
    Quaternion  invPlanetAxis = d->m_viewParams.m_planetAxis.inverse();

    northPole.rotateAroundAxis(invPlanetAxis);

    return (int)( d->m_viewParams.m_radius * northPole.v[Q_Y] );
}

int MarbleWidget::northPoleZ()
{
    Quaternion  northPole     = GeoPoint( 0.0, -M_PI * 0.5 ).quaternion();
    Quaternion  invPlanetAxis = d->m_viewParams.m_planetAxis.inverse();

    northPole.rotateAroundAxis(invPlanetAxis);

    return (int)( d->m_viewParams.m_radius * northPole.v[Q_Z] );
}

bool MarbleWidget::screenCoordinates( const double lng, const double lat,
                                      int& x, int& y )
{
    Quaternion  qpos       = GeoPoint( lng, lat ).quaternion();
    Quaternion  invRotAxis = d->m_viewParams.m_planetAxis.inverse();

    qpos.rotateAroundAxis(invRotAxis);

    x = (int)(  d->m_viewParams.m_radius * qpos.v[Q_X] );
    y = (int)( -d->m_viewParams.m_radius * qpos.v[Q_Y] );

    if ( qpos.v[Q_Z] >= 0.0 )
        return true;
    else
        return false;
}


bool MarbleWidget::globeSphericals(int x, int y, double& alpha, double& beta)
{

    int imgrx  = width() >> 1;
    int imgry  = height() >> 1;

    const double  radiusf = 1.0 / (double)(radius());

    if ( radius() > sqrt((x - imgrx)*(x - imgrx) + (y - imgry)*(y - imgry)) ) {

	double qy = radiusf * (double)(y - imgry);
	double qr = 1.0 - qy * qy;
	double qx = (double)(x - imgrx) * radiusf;

	double qr2z = qr - qx * qx;
	double qz = (qr2z > 0.0) ? sqrt( qr2z ) : 0.0;	

	Quaternion  qpos( 0, qx, qy, qz );
	qpos.rotateAroundAxis( planetAxis() );
	qpos.getSpherical( alpha, beta );

	return true;
    }
    else {
	return false;
    }
}


void MarbleWidget::rotateTo(const uint& phi, const uint& theta, const uint& psi)
{
    d->m_viewParams.m_planetAxis.createFromEuler( (double)(phi)   / RAD2INT,
                                                  (double)(theta) / RAD2INT,
                                                  (double)(psi)   / RAD2INT );
}

void MarbleWidget::rotateTo(const double& lat, const double& lon)
{
    d->m_viewParams.m_planetAxis.createFromEuler( (lat + 180.0) * M_PI / 180.0,
                                                  (lon + 180.0) * M_PI / 180.0, 0.0 );
}


void MarbleWidget::drawAtmosphere()
{
    int  imgrx  = width() / 2;
    int  imgry  = height() / 2;

    // Recalculate the atmosphere effect and paint it to canvasImage.
    QRadialGradient grad1( QPointF( imgrx, imgry ), 1.05 * radius() );
    grad1.setColorAt( 0.91, QColor( 255, 255, 255, 255 ) );
    grad1.setColorAt( 1.0,  QColor( 255, 255, 255, 0 ) );

    QBrush    brush1( grad1 );
    QPen    pen1( Qt::NoPen );
    QPainter  painter( d->m_viewParams.m_canvasImage );
    painter.setBrush( brush1 );
    painter.setPen( pen1 );
    painter.setRenderHint( QPainter::Antialiasing, false );
    painter.drawEllipse( imgrx - (int)( (double)(radius()) * 1.05 ),
                         imgry - (int)( (double)(radius()) * 1.05 ),
                         (int)( 2.1 * (double)(radius()) ), 
                         (int)( 2.1 * (double)(radius()) ) );
}


void MarbleWidget::setActiveRegion()
{
    int zoom = radius(); 

    d->m_activeRegion = QRegion( 25, 25, width() - 50, height() - 50, 
                                 QRegion::Rectangle );
#ifndef FLAT_PROJ
    if ( zoom < sqrt( width() * width() + height() * height() ) / 2 ) {
	d->m_activeRegion &= QRegion( width() / 2 - zoom, height() / 2 - zoom, 
                                      2 * zoom, 2 * zoom, QRegion::Ellipse );
    }
#else
    double centerLat = planetAxis().pitch();
    int yCenterOffset =  (int)((float)(2*zoom / M_PI) * centerLat);
    int yTop = height()/2 - zoom + yCenterOffset;

    d->m_activeRegion &= QRegion( 0, yTop, width(), 2*zoom, QRegion::Rectangle );
#endif
}

const QRegion MarbleWidget::activeRegion()
{
    return d->m_activeRegion;
}


void MarbleWidget::paintEvent(QPaintEvent *evt)
{
    //	Debugging Active Region
    //	painter.setClipRegion(activeRegion);

    //	if ( d->m_model->needsUpdate()
    //       || d->m_viewParams.m_pCanvasImage->isNull()
    //       || d->m_viewParams.m_pCanvasImage->size() != size()) {

    bool  doClip = ( d->m_viewParams.m_radius > d->m_viewParams.m_canvasImage->width() / 2
                     || d->m_viewParams.m_radius > d->m_viewParams.m_canvasImage->height() / 2 );

    // Create a painter that will do the painting.
    ClipPainter painter( this, doClip); 

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

    // 2. Paint the scale.
    if ( d->m_showScaleBar == true )
        painter.drawPixmap( 10, d->m_viewParams.m_canvasImage->height() - 40,
                            d->m_mapscale.drawScaleBarPixmap( radius(),
                                                              d->m_viewParams.m_canvasImage-> width() / 2 - 20 ) );

    // 3. Paint the wind rose.
    if ( d->m_showWindRose == true )
        painter.drawPixmap( d->m_viewParams.m_canvasImage->width() - 60, 10,
                            d->m_windrose.drawWindRosePixmap( d->m_viewParams.m_canvasImage->width(),
                                                              d->m_viewParams.m_canvasImage->height(),
                                                              northPoleY() ) );

    // 4. Paint the crosshair.
    d->m_crosshair.paintCrossHair( &painter, 
                                   d->m_viewParams.m_canvasImage->width(),
                                   d->m_viewParams.m_canvasImage->height() );

    // 5. Paint measure points if there are any.
    d->m_measureTool->paintMeasurePoints( &painter, 
                                          d->m_viewParams.m_canvasImage->width() / 2,
                                          d->m_viewParams.m_canvasImage->height() / 2,
                                          radius(), planetAxis(),
                                          true );

    // Set the region of the image where the user can drag it.
    setActiveRegion();
}


void MarbleWidget::goHome()
{
    // d->m_model->rotateTo(0, 0);
#if 1
    rotateTo( 54.8, -9.4 );
#else
    rotateTo( d->m_homePoint.quaternion() );
#endif
    zoomView( d->m_homeZoom ); // default 1050

    repaint(); // not obsolete in case the zoomlevel stays unaltered
}


void MarbleWidget::setMapTheme( const QString& maptheme )
{
    d->m_model->setMapTheme( maptheme, this );
    setNeedsUpdate();
    repaint();
}

void MarbleWidget::setShowScaleBar( bool visible )
{ 
    d->m_showScaleBar = visible;
    repaint();
}

void MarbleWidget::setShowWindRose( bool visible )
{ 
    d->m_showWindRose = visible;
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

void MarbleWidget::setShowRelief( bool visible )
{ 
    d->m_model->textureColorizer()->setShowRelief( visible );
    setNeedsUpdate();
    repaint();
}

void MarbleWidget::setShowElevationModel( bool visible )
{ 
    d->m_viewParams.m_showElevationModel = visible;
    setNeedsUpdate();
    repaint();
}

void MarbleWidget::setShowIceLayer( bool visible )
{ 
    d->m_model->vectorComposer()->setShowIceLayer( visible );
    setNeedsUpdate();
    repaint();
}

void MarbleWidget::setShowBorders( bool visible )
{ 
    d->m_model->vectorComposer()->setShowBorders( visible );
    repaint();
}

void MarbleWidget::setShowRivers( bool visible )
{ 
    d->m_model->vectorComposer()->setShowRivers( visible );
    repaint();
}

void MarbleWidget::setShowLakes( bool visible )
{
    d->m_model->vectorComposer()->setShowLakes( visible );
    setNeedsUpdate();
    repaint();
}

void MarbleWidget::setShowGps( bool visible )
{
    d->m_model->gpsLayer()->setVisible( visible );
    repaint();
}

void MarbleWidget::changeGpsPosition( double lat, double lon)
{
    d->m_model->gpsLayer()->changeCurrentPosition( lat, lon );
    repaint();
}

void MarbleWidget::gpsCoordinatesClick( int x, int y)
{
    bool valid = false;
    double alphaResult=0,betaResult=0;
    
    valid = globeSphericals( x, y, alphaResult, betaResult );
    
    if (valid){
        emit gpsClickPos( alphaResult, betaResult, GeoPoint::Radian);
    }
}

void MarbleWidget::updateGps()
{
    d->m_model->gpsLayer()->updateGps();
    repaint();
}

void MarbleWidget::openGpxFile(QString &filename)
{
    d->m_model->gpsLayer()->loadGpx( filename );
}

void MarbleWidget::setQuickDirty( bool enabled )
{
#ifndef FLAT_PROJ
    // Interlace texture mapping 
    d->m_model->textureMapper()->setInterlaced( enabled );
    setNeedsUpdate();

    int transparency = enabled ? 255 : 192;
    d->m_windrose.setTransparency( transparency );
    d->m_mapscale.setTransparency( transparency );
    repaint();
#endif
}


// This slot will called when the Globe starts to create the tiles.

void MarbleWidget::creatingTilesStart( const QString &name, const QString &description )
{
    qDebug("MarbleWidget::creatingTilesStart called... ");

    d->m_tileCreatorDlg = new KAtlasTileCreatorDialog( this );

    d->m_tileCreatorDlg->setSummary( name, description );

    // The process itself is started by a timer, so an exec() is ok here.
    d->m_tileCreatorDlg->exec();
    qDebug("MarbleWidget::creatingTilesStart exits... ");
}

// This slot will be called during the tile creation progress.  When
// the progress goes to 100, the dialog should be closed.

void MarbleWidget::creatingTilesProgress( int progress )
{
    d->m_tileCreatorDlg->setProgress( progress );

    if ( progress == 100 )
        delete d->m_tileCreatorDlg;
}

void MarbleWidget::updateChangedMap()
{
    setNeedsUpdate();
    update();
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

#include "MarbleWidget.moc"
