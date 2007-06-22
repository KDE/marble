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

#include <QtCore/QAbstractItemModel>
#include <QtCore/QTime>
#include <QtGui/QSizePolicy>
#include <QtGui/QRegion>

#include "Quaternion.h"
#include "texcolorizer.h"
#include "clippainter.h"
#include "katlasviewinputhandler.h"
#include "katlasviewpopupmenu.h"
#include "katlastilecreatordialog.h"

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

    GeoPoint         m_homePoint;

    int              m_logzoom;
	
    int              m_zoomStep;
    int              m_minimumzoom;    
    int              m_maximumzoom;

    KAtlasViewInputHandler  *m_inputhandler;
    KAtlasViewPopupMenu     *m_popupmenu;

    TextureColorizer        *m_sealegend;
    QImage                  *m_pCanvasImage;

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

void MarbleWidget::construct(QWidget *parent)
{
    setMinimumSize( 200, 300 );
    setFocusPolicy( Qt::WheelFocus );
    setFocus( Qt::OtherFocusReason );

    // Some point that tackat defined. :-) 
    setHomePoint( 54.8, -9.4 );

    connect( d->m_model, SIGNAL( creatingTilesStart( const QString&, const QString& ) ),
             this,    SLOT( creatingTilesStart( const QString&, const QString& ) ) );
    connect( d->m_model, SIGNAL( creatingTilesProgress( int ) ),
             this,    SLOT( creatingTilesProgress( int ) ) );

    connect( d->m_model, SIGNAL(themeChanged()), this, SLOT(update()) );

    // Set background: black.
    QPalette p = palette();
    p.setColor( QPalette::Window, Qt::black );
    setPalette( p );
    setBackgroundRole( QPalette::Window );
    setAutoFillBackground( true );

    //	setAttribute(Qt::WA_NoSystemBackground);

    d->m_pCanvasImage = new QImage( parent->width(), parent->height(),
				 QImage::Format_ARGB32_Premultiplied );
    d->m_model->setCanvasImage( d->m_pCanvasImage );

    d->m_inputhandler = new KAtlasViewInputHandler( this, d->m_model );
    installEventFilter( d->m_inputhandler );
    setMouseTracking( true );

    d->m_popupmenu = new KAtlasViewPopupMenu( this, d->m_model );
    connect( d->m_inputhandler, SIGNAL( lmbRequest( int, int ) ),
	     d->m_popupmenu,    SLOT( showLmbMenu( int, int ) ) );	
    connect( d->m_inputhandler, SIGNAL( rmbRequest( int, int ) ),
	     d->m_popupmenu,    SLOT( showRmbMenu( int, int ) ) );	

    connect( d->m_inputhandler, SIGNAL( mouseGeoPosition( QString ) ),
         this, SIGNAL( mouseGeoPosition( QString ) ) ); 

    d->m_measureTool = new MeasureTool( this );

    connect( d->m_popupmenu,   SIGNAL( addMeasurePoint( double, double ) ),
	     d->m_measureTool, SLOT( addMeasurePoint( double, double ) ) );
    connect( d->m_popupmenu,   SIGNAL( removeMeasurePoints() ),
	     d->m_measureTool, SLOT( removeMeasurePoints( ) ) );	

    d->m_logzoom  = 0;
    d->m_zoomStep = 40;
    goHome();

    d->m_minimumzoom = 950;
    d->m_maximumzoom = 2200;

    d->m_showScaleBar = true;
    d->m_showWindRose = true;
}

MarbleModel *MarbleWidget::model() const
{
    return d->m_model;
}


QAbstractListModel *MarbleWidget::placeMarkModel()
{
    return d->m_model->getPlaceMarkModel();
}

double MarbleWidget::moveStep()
{
    if ( d->m_model->radius() < sqrt( width() * width() + height() * height() ) )
	return 0.1;
    else
	return atan( (double)width() 
		     / (double)( 2 * d->m_model->radius() ) ) * 0.2;
}

int MarbleWidget::zoom() const
{
    return d->m_logzoom; 
}

double MarbleWidget::centerLatitude()
{ 
    return d->m_model->centerLatitude();
}

double MarbleWidget::centerLongitude()
{
    return d->m_model->centerLongitude();
}

void MarbleWidget::setMinimumZoom( int zoom )
{
    d->m_minimumzoom = zoom; 
}

void MarbleWidget::addPlaceMarkFile( QString filename )
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
    return d->m_model->showGrid();
}

bool MarbleWidget::showPlaces() const
{ 
    return d->m_model->showPlaceMarks();
}

bool MarbleWidget::showCities() const
{ 
    return d->m_model->placeMarkPainter()->showCities();
}

bool MarbleWidget::showTerrain() const
{ 
    return d->m_model->placeMarkPainter()->showTerrain();
}

bool MarbleWidget::showRelief() const
{ 
    return d->m_model->textureColorizer()->showRelief();
}

bool MarbleWidget::showElevationModel() const
{ 
    return d->m_model->showElevationModel();
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

bool  MarbleWidget::quickDirty() const
{ 
    return d->m_model->textureMapper()->interlaced();
}


void MarbleWidget::zoomView(int zoom)
{
    // Prevent infinite loops.
    if ( zoom  == d->m_logzoom )
	return;

    d->m_logzoom = zoom;

    emit zoomChanged(zoom);

    int radius = fromLogScale(zoom);

    if ( radius == d->m_model->radius() )
	return;
	
    d->m_model->setRadius(radius);
    repaint();

    setActiveRegion();
}


void MarbleWidget::zoomViewBy(int zoomStep)
{
    // Prevent infinite loops

    int zoom = d->m_model->radius();
    int tryZoom = toLogScale(zoom) + zoomStep;
    //	qDebug() << QString::number(tryZoom) << " " << QString::number(minimumzoom);
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

void MarbleWidget::rotateBy(const double& phi, const double& theta)
{
    d->m_model->rotateBy( phi, theta );

    repaint();
}

void MarbleWidget::centerOn(const double& lat, const double& lon)
{
    d->m_model->rotateTo( lat, lon );

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

    // FIXME: Remove
    //d->m_model->placeMarkContainer()->clearTextPixmaps();
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

void MarbleWidget::moveLeft()
{
    rotateBy( 0, moveStep() );
}

void MarbleWidget::moveRight()
{
    rotateBy( 0, -moveStep() );
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
    delete d->m_pCanvasImage;

    d->m_pCanvasImage = new QImage( width(), height(),
				 QImage::Format_ARGB32_Premultiplied );
    d->m_model->setCanvasImage( d->m_pCanvasImage );
    d->m_model->resize();

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

bool MarbleWidget::globeSphericals(int x, int y, double& alpha, double& beta)
{

    int radius = d->m_model->radius(); 
    int imgrx  = width() >> 1;
    int imgry  = height() >> 1;

    const double  radiusf = 1.0 / (double)(radius);

    if ( radius > sqrt((x - imgrx)*(x - imgrx) + (y - imgry)*(y - imgry)) ) {

	double qy = radiusf * (double)(y - imgry);
	double qr = 1.0 - qy * qy;
	double qx = (double)(x - imgrx) * radiusf;

	double qr2z = qr - qx * qx;
	double qz = (qr2z > 0.0) ? sqrt( qr2z ) : 0.0;	

	Quaternion  qpos( 0, qx, qy, qz );
	qpos.rotateAroundAxis( d->m_model->getPlanetAxis() );
	qpos.getSpherical( alpha, beta );

	return true;
    }
    else {
	return false;
    }
}

void MarbleWidget::setActiveRegion()
{
    int zoom = d->m_model->radius(); 

    d->m_activeRegion = QRegion( 25, 25, width() - 50, height() - 50, 
                                 QRegion::Rectangle );

    if ( zoom < sqrt( width() * width() + height() * height() ) / 2 ) {
	d->m_activeRegion &= QRegion( width() / 2 - zoom, height() / 2 - zoom, 
                                      2 * zoom, 2 * zoom, QRegion::Ellipse );
    }
}

const QRegion MarbleWidget::activeRegion()
{
    return d->m_activeRegion;
}


void MarbleWidget::paintEvent(QPaintEvent *evt)
{
    //	Debugging Active Region
    //	painter.setClipRegion(activeRegion);

    //	if (d->m_model->needsUpdate() || d->m_pCanvasImage->isNull() || d->m_pCanvasImage->size() != size())
    //	{

    int   radius = d->m_model->radius();
    bool  clip = ( radius > d->m_pCanvasImage->width()/2
                   || radius > d->m_pCanvasImage->height()/2 ) ? true : false;

    // Paint the globe itself.
    ClipPainter painter( this, clip); 
    // QPainter painter(this);
    // painter.setClipRect(10, 10, d->m_pCanvasImage->width() - 1 , d->m_pCanvasImage->height()-1 );
    // painter.setClipping( true );
    // painter.clearNodeCount();

    QRect  dirty = evt->rect();
    d->m_model->paintGlobe(&painter,dirty);
	
    // Draw the scale.
    if ( d->m_showScaleBar == true )
        painter.drawPixmap( 10, d->m_pCanvasImage->height() - 40,
                            d->m_mapscale.drawScaleBarPixmap( d->m_model->radius(),
                                                       d->m_pCanvasImage-> width() / 2 - 20 ) );

    // Draw the wind rose.
    if ( d->m_showWindRose == true )
        painter.drawPixmap( d->m_pCanvasImage->width() - 60, 10,
    			d->m_windrose.drawWindRosePixmap( d->m_pCanvasImage->width(),
						       d->m_pCanvasImage->height(),
                                                       d->m_model->northPoleY() ) );

    // Draw the crosshair.
    d->m_crosshair.paintCrossHair( &painter, 
				d->m_pCanvasImage->width(),
                                d->m_pCanvasImage->height() );

    d->m_measureTool->paintMeasurePoints( &painter, 
                                          d->m_pCanvasImage->width() / 2,
                                          d->m_pCanvasImage->height() / 2,
                                          radius, d->m_model->getPlanetAxis(),
                                          true );
#if 0
      else
      {
      // Draw cached pixmap to widget
      QPainter pixmapPainter(this);
      QRect rect(0, 0, width(), height());
      pixmapPainter.drawImage(rect, d->m_pCanvasImage, rect);
      }
#endif
}


void MarbleWidget::goHome()
{
    // d->m_model->rotateTo(0, 0);
    d->m_model->rotateTo( 54.8, -9.4 );
    zoomView( 1050 ); // default 1050

    update(); // not obsolete in case the zoomlevel stays unaltered
}


void MarbleWidget::setMapTheme( const QString& maptheme )
{
    d->m_model->setMapTheme( maptheme );
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
    d->m_model->setShowGrid( visible );
    repaint();
}

void MarbleWidget::setShowPlaces( bool visible )
{ 
    d->m_model->setShowPlaceMarks( visible );
    repaint();
}

void MarbleWidget::setShowCities( bool visible )
{ 
    d->m_model->placeMarkPainter()->setShowCities( visible );
    repaint();
}

void MarbleWidget::setShowTerrain( bool visible )
{ 
    d->m_model->placeMarkPainter()->setShowTerrain( visible );
    repaint();
}

void MarbleWidget::setShowRelief( bool visible )
{ 
    d->m_model->textureColorizer()->setShowRelief( visible );
    d->m_model->setNeedsUpdate();
    repaint();
}

void MarbleWidget::setShowElevationModel( bool visible )
{ 
    d->m_model->setShowElevationModel( visible );
    d->m_model->setNeedsUpdate();
    repaint();
}

void MarbleWidget::setShowIceLayer( bool visible )
{ 
    d->m_model->vectorComposer()->setShowIceLayer( visible );
    d->m_model->setNeedsUpdate();
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
    repaint();
}

void MarbleWidget::setQuickDirty( bool enabled )
{
    // Interlace texture mapping 
    d->m_model->textureMapper()->setInterlaced( enabled );
    d->m_model->setNeedsUpdate();

    int transparency = enabled ? 255 : 192;
    d->m_windrose.setTransparency( transparency );
    d->m_mapscale.setTransparency( transparency );
    repaint();
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
