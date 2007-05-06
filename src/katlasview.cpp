#include "katlasview.h"

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
  static double sqrt(int a) { return sqrt((double)a); }
#endif


KAtlasView::KAtlasView(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize( 200, 300 );
    setFocusPolicy( Qt::WheelFocus );
    setFocus( Qt::OtherFocusReason );
    //FIXME(ModelView): Provide this to the constructor

    m_pGlobe = new KAtlasGlobe( this );
    connect( m_pGlobe, SIGNAL( creatingTilesStart( QString, QString ) ),
             this,     SLOT( creatingTilesStart( QString, QString ) ) );
    connect( m_pGlobe, SIGNAL( creatingTilesProgress( int ) ),
             this,     SLOT( creatingTilesProgress( int ) ) );

    connect( m_pGlobe, SIGNAL(themeChanged()), this, SLOT(update()) );

    // Set background: black.
    QPalette p = palette();
    p.setColor( QPalette::Window, Qt::black );
    setPalette( p );
    setBackgroundRole( QPalette::Window );
    setAutoFillBackground( true );

    //	setAttribute(Qt::WA_NoSystemBackground);

    m_pCanvasImage = new QImage( parent->width(), parent->height(),
				 QImage::Format_ARGB32_Premultiplied );
    m_pGlobe->setCanvasImage( m_pCanvasImage );

    m_inputhandler = new KAtlasViewInputHandler( this, m_pGlobe );
    installEventFilter( m_inputhandler );
    setMouseTracking( true );

    m_popupmenu = new KAtlasViewPopupMenu( this, m_pGlobe );
    connect( m_inputhandler, SIGNAL( lmbRequest( int, int ) ),
	     m_popupmenu,    SLOT( showLmbMenu( int, int ) ) );	
    connect( m_inputhandler, SIGNAL( rmbRequest( int, int ) ),
	     m_popupmenu,    SLOT( showRmbMenu( int, int ) ) );	

    m_pMeasureTool = new MeasureTool( this );

    connect( m_popupmenu,    SIGNAL( addMeasurePoint( float, float ) ),
	     m_pMeasureTool, SLOT( addMeasurePoint( float, float ) ) );	
    connect( m_popupmenu,    SIGNAL( removeMeasurePoints() ),
	     m_pMeasureTool, SLOT( removeMeasurePoints( ) ) );	

    m_logzoom  = 0;
    m_zoomStep = 40;
    goHome();
    m_minimumzoom = 50;
}


void KAtlasView::zoomView(int zoom)
{
    // Prevent infinite loops.
    if ( zoom  == m_logzoom )
	return;

    m_logzoom = zoom;

    emit zoomChanged(zoom);

    int radius = fromLogScale(zoom);

    if ( radius == m_pGlobe->radius() )
	return;
	
    m_pGlobe->setRadius(radius);
    repaint();

    setActiveRegion();
}


void KAtlasView::zoomViewBy(int zoomstep)
{
    // Prevent infinite loops

    int zoom = m_pGlobe->radius();
    int tryZoom = toLogScale(zoom) + zoomstep;
    //	qDebug() << QString::number(tryZoom) << " " << QString::number(minimumzoom);
    if ( tryZoom >= m_minimumzoom ) {
	zoom = tryZoom;
	zoomView(zoom);
    }
}


void KAtlasView::zoomIn()
{
    zoomViewBy( m_zoomStep );
}

void KAtlasView::zoomOut()
{
    zoomViewBy( -m_zoomStep );
}

void KAtlasView::rotateBy(const float& phi, const float& theta)
{
    m_pGlobe->rotateBy( phi, theta );

    repaint();
}

void KAtlasView::centerOn(const float& phi, const float& theta)
{
    m_pGlobe->rotateTo( phi, theta );

    repaint();
}

void KAtlasView::centerOn(const QModelIndex& index)
{

    PlaceMarkModel* model = (PlaceMarkModel*) m_pGlobe->getPlaceMarkModel();
    if (model == 0) qDebug( "model null" );

    PlaceMark* mark = model->placeMark( index );

    m_pGlobe->placeContainer()->clearSelected();

    if ( mark != 0 ){
	float  lon;
        float  lat;

	mark->coordinate( lon, lat );
	centerOn( -lat * 180.0 / M_PI, -lon * 180.0 / M_PI );
	mark->setSelected( 1 );
	m_crosshair.setEnabled( true );
    }
    else 
	m_crosshair.setEnabled( false );

    m_pGlobe->placeContainer()->clearTextPixmaps();
    m_pGlobe->placeContainer()->sort();

    repaint();
}


void KAtlasView::moveLeft()
{
    rotateBy( 0, moveStep() );
}

void KAtlasView::moveRight()
{
    rotateBy( 0, -moveStep() );
}


void KAtlasView::moveUp()
{
    rotateBy( moveStep(), 0 );
}

void KAtlasView::moveDown()
{
    rotateBy( -moveStep(), 0 );
}

void KAtlasView::resizeEvent (QResizeEvent*)
{
    //	Redefine the area where the mousepointer becomes a navigationarrow
    setActiveRegion();
    if ( m_pCanvasImage != 0 ) 
	delete m_pCanvasImage;

    m_pCanvasImage = new QImage( width(), height(),
				 QImage::Format_ARGB32_Premultiplied );
    m_pGlobe->setCanvasImage( m_pCanvasImage );
    m_pGlobe->resize();

    repaint();
}


bool KAtlasView::globeSphericals(int x, int y, float& alpha, float& beta)
{

    int radius = m_pGlobe->radius(); 
    int imgrx  = width() >> 1;
    int imgry  = height() >> 1;

    const float  radiusf = 1.0 / (float)(radius);

    if ( radius > sqrt((x - imgrx)*(x - imgrx) + (y - imgry)*(y - imgry)) ) {

	float qy = radiusf * (float)(y - imgry);
	float qr = 1.0 - qy * qy;
	float qx = (float)(x - imgrx) * radiusf;

	float qr2z = qr - qx * qx;
	float qz = (qr2z > 0.0) ? sqrt( qr2z ) : 0.0;	

	Quaternion  qpos( 0, qx, qy, qz );
	qpos.rotateAroundAxis( m_pGlobe->getPlanetAxis() );
	qpos.getSpherical( alpha, beta );

	return true;
    }
    else {
	return false;
    }
}

void KAtlasView::setActiveRegion()
{
    int zoom = m_pGlobe->radius(); 

    m_activeRegion = QRegion( 25, 25, width() - 50, height() - 50, 
                              QRegion::Rectangle );

    if ( zoom < sqrt( width() * width() + height() * height() ) / 2 ) {
	m_activeRegion &= QRegion( width() / 2 - zoom, height() / 2 - zoom, 
                                   2 * zoom, 2 * zoom, QRegion::Ellipse );
    }
}

const QRegion KAtlasView::activeRegion()
{
    return m_activeRegion;
}


void KAtlasView::paintEvent(QPaintEvent *evt)
{
    //	Debugging Active Region
    //	painter.setClipRegion(activeRegion);

    //	if(m_pGlobe->needsUpdate() || m_pCanvasImage->isNull() || m_pCanvasImage->size() != size())
    //	{

    int   radius = m_pGlobe->radius();
    bool  clip = ( radius > m_pCanvasImage->width()/2
                   || radius > m_pCanvasImage->height()/2 ) ? true : false;

    // Paint the globe itself.
    ClipPainter painter( this, clip); 
    // QPainter painter(this);
    // painter.setClipRect(10, 10, m_pCanvasImage->width() - 1 , m_pCanvasImage->height()-1 );
    // painter.setClipping( true );
    // painter.clearNodeCount();

    QRect  dirty = evt->rect();
    m_pGlobe->paintGlobe(&painter,dirty);
	
    // Draw the scale.
    painter.drawPixmap( 10, m_pCanvasImage->height() - 40,
                        m_mapscale.drawScaleBarPixmap( m_pGlobe->radius(),
                                                       m_pCanvasImage-> width() / 2 - 20 ) );

    // Draw the wind rose.
    painter.drawPixmap( m_pCanvasImage->width() - 60, 10,
			m_windrose.drawWindRosePixmap( m_pCanvasImage->width(),
						       m_pCanvasImage->height(),
                                                       m_pGlobe->northPoleY() ) );

    // Draw the crosshair.
    m_crosshair.paintCrossHair( &painter, 
				m_pCanvasImage->width(),
                                m_pCanvasImage->height() );

    m_pMeasureTool->paintMeasurePoints( &painter, m_pCanvasImage->width() / 2,
					m_pCanvasImage->height() / 2,
					radius, m_pGlobe->getPlanetAxis(),
                                        true );
#if 0
      else
      {
      // Draw cached pixmap to widget
      QPainter pixmapPainter(this);
      QRect rect(0, 0, width(), height());
      pixmapPainter.drawImage(rect, m_pCanvasImage, rect);
      }
#endif
}


void KAtlasView::goHome()
{
    // m_pGlobe->rotateTo(0, 0);
    m_pGlobe->rotateTo( 54.8, -9.4 );
    zoomView( 1050 ); // default 1050

    update(); // not obsolete in case the zoomlevel stays unaltered
}


// This slot will called when the Globe starts to create the tiles.

void KAtlasView::creatingTilesStart( QString name, QString description )
{
    qDebug("KAtlasView::creatingTilesStart called... ");

    m_tileCreatorDlg = new KAtlasTileCreatorDialog( this );

    m_tileCreatorDlg->setSummary( name, description );

    // The process itself is started by a timer, so an exec() is ok here.
    m_tileCreatorDlg->exec();
    qDebug("KAtlasView::creatingTilesStart exits... ");
}

// This slot will be called during the tile creation progress.  When
// the progress goes to 100, the dialog should be closed.

void KAtlasView::creatingTilesProgress( int progress )
{
    m_tileCreatorDlg->setProgress( progress );

    if ( progress == 100 )
        delete m_tileCreatorDlg;
}


float KAtlasView::moveStep()
{
    if ( m_pGlobe->radius() < sqrt( width() * width() + height() * height() ) )
	return 0.1f;
    else
	return atanf( (float)width() 
                      / (float)( 2 * m_pGlobe->radius() ) ) * 0.2f;
}

int KAtlasView::fromLogScale(int zoom)
{
    zoom = (int) pow(M_E, ((float)zoom / 200.));
    // zoom = (int) pow(2, ((float)zoom/200));
    return zoom;
}

int KAtlasView::toLogScale(int zoom)
{
    zoom = (int)(200.0f * logf( (float)zoom ) );
    return zoom;
}

#ifndef Q_OS_MACX
#include "katlasview.moc"
#endif
