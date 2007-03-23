#include "katlasview.h"

#include <cmath>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QSizePolicy>
#include <QtCore/QTime>
#include <QtGui/QSizePolicy>
#include <QtGui/QRegion>

#include "clippainter.h"
#include "katlasviewinputhandler.h"
#include "katlasviewpopupmenu.h"
#include "quaternion.h"
#include "texcolorizer.h"

#include "measuretool.h"

#ifdef Q_CC_MSVC
  static double sqrt(int a) { return sqrt((double)a); }
#endif

KAtlasView::KAtlasView(QWidget *parent)
    : QWidget(parent)
{
	setMinimumSize(200, 300);
	setFocusPolicy(Qt::WheelFocus);
        setFocus(Qt::OtherFocusReason);
	m_pGlobe = new KAtlasGlobe( this );

	QPalette p = palette();
	p.setColor(QPalette::Window,Qt::black);
	setPalette(p);

	setBackgroundRole(QPalette::Window);
	setAutoFillBackground(true);

//	setAttribute(Qt::WA_NoSystemBackground);

	m_pCanvasImage = new QImage(parent->width(),parent->height(),QImage::Format_ARGB32_Premultiplied);
	m_pGlobe->setCanvasImage( m_pCanvasImage );

	inputhandler = new KAtlasViewInputHandler(this, m_pGlobe);
	installEventFilter(inputhandler);
	setMouseTracking(true);

	m_popupmenu = new KAtlasViewPopupMenu(this, m_pGlobe);
	connect( inputhandler, SIGNAL( lmbRequest( int, int ) ), m_popupmenu, SLOT( showLmbMenu( int, int ) ) );	
	connect( inputhandler, SIGNAL( rmbRequest( int, int ) ), m_popupmenu, SLOT( showRmbMenu( int, int ) ) );	

	m_pMeasureTool = new MeasureTool( this );

	connect( m_popupmenu, SIGNAL( addMeasurePoint( float, float ) ), m_pMeasureTool, SLOT( addMeasurePoint( float, float ) ) );	
	connect( m_popupmenu, SIGNAL( removeMeasurePoints() ), m_pMeasureTool, SLOT( removeMeasurePoints( ) ) );	

	m_logzoom = 0;
	m_zoomStep = 40;
	goHome();
	minimumzoom = 50;
}

void KAtlasView::zoomView(int zoom){
	// prevent infinite loops

	if ( zoom  == m_logzoom )
		return;

	m_logzoom = zoom;

	emit zoomChanged(zoom);

	int radius = fromLogScale(zoom);

	if ( radius == m_pGlobe->getRadius() )
		return;
	
	m_pGlobe->setRadius(radius);
	repaint();

	setActiveRegion();
}

void KAtlasView::zoomViewBy(int zoomstep){
	// prevent infinite loops

	int zoom = m_pGlobe->getRadius();
	int tryZoom = toLogScale(zoom) + zoomstep;
//	qDebug() << QString::number(tryZoom) << " " << QString::number(minimumzoom);
	if ( tryZoom >= minimumzoom ) {
		zoom = tryZoom;
		zoomView(zoom);
	}
}

void KAtlasView::zoomIn(){
	zoomViewBy(m_zoomStep);
}

void KAtlasView::zoomOut(){
	zoomViewBy(-m_zoomStep);
}

void KAtlasView::rotateBy(const float& phi, const float& theta){
	m_pGlobe->rotateBy(phi, theta);

	repaint();
}

void KAtlasView::centerOn(const float& phi, const float& theta){
	m_pGlobe->rotateTo(phi, theta);

	repaint();
}

void KAtlasView::centerOn(const QModelIndex& index){

	PlaceMarkModel* model = (PlaceMarkModel*)m_pGlobe->getPlaceMarkModel();
	if (model == 0) qDebug("model null");

	PlaceMark* mark = model->placeMark( index );

	m_pGlobe->placeContainer()->clearSelected();

	if (mark != 0){
		float lng, lat;
		mark->coordinate(lng, lat);
		centerOn(-lat*180.0/M_PI, -lng*180.0/M_PI);
		mark->setSelected(1);
		m_crosshair.setEnabled( true );
	}
	else 
		m_crosshair.setEnabled( false );

	m_pGlobe->placeContainer()->clearTextPixmaps();
	m_pGlobe->placeContainer()->sort();

	repaint();
}

void KAtlasView::moveLeft(){
	rotateBy(0, getMoveStep());
}

void KAtlasView::moveRight(){
	rotateBy(0, -getMoveStep());
}

void KAtlasView::moveUp(){
	rotateBy(getMoveStep(), 0);
}

void KAtlasView::moveDown(){
	rotateBy(-getMoveStep(), 0);
}

void KAtlasView::resizeEvent (QResizeEvent*){
//	Redefine the area where the mousepointer becomes a navigationarrow
	setActiveRegion();
	if ( m_pCanvasImage != 0 ) delete m_pCanvasImage;
	m_pCanvasImage = new QImage(width(),height(),QImage::Format_ARGB32_Premultiplied);
	m_pGlobe->setCanvasImage( m_pCanvasImage );
	m_pGlobe->resize();

	repaint();
}

bool KAtlasView::getGlobeSphericals(int x, int y, float& alpha, float& beta){

	int radius = m_pGlobe->getRadius(); 
	int imgrx = width() >> 1;
	int imgry = height() >> 1;

	const float radiusf = 1.0/(float)(radius);

	if ( radius > sqrt((x - imgrx)*(x - imgrx) + (y - imgry)*(y - imgry)) ) {

		float qy = radiusf * (float)(y - imgry);
		float qr = 1.0 - qy*qy;
		float qx = (float)(x - imgrx) * radiusf;

		float qr2z = qr - qx*qx;
		float qz = (qr2z > 0.0) ? sqrt(qr2z) : 0.0;	

		Quaternion qpos(0,qx,qy,qz);
		qpos.rotateAroundAxis(m_pGlobe->getPlanetAxis());
		qpos.getSpherical( alpha, beta );

		return true;
	}
	else {
		return false;
	}
}

void KAtlasView::setActiveRegion(){
	int zoom = m_pGlobe->getRadius(); 

	activeRegion = QRegion(25,25,width()-50,height()-50, QRegion::Rectangle);

	if ( zoom < sqrt(width()*width()+ height()*height())/2){
		activeRegion &= QRegion(width()/2-zoom, height()/2-zoom, 2*zoom, 2*zoom, QRegion::Ellipse);
	}
}

const QRegion KAtlasView::getActiveRegion(){
	return activeRegion;
}

void KAtlasView::paintEvent(QPaintEvent *evt)
{
//	QTime timer;
//	timer.restart();
//	Debugging Active Region
//	painter.setClipRegion(activeRegion);

//	if(m_pGlobe->needsUpdate() || m_pCanvasImage->isNull() || m_pCanvasImage->size() != size())
//	{
		int radius = m_pGlobe->getRadius();
		bool clip = (radius > m_pCanvasImage->width()/2 || radius > m_pCanvasImage->height()/2) ? true : false;

		ClipPainter painter( this, clip); 
//		QPainter painter(this);
//		painter.setClipRect(10, 10, m_pCanvasImage->width() - 1 , m_pCanvasImage->height()-1 );
//		painter.setClipping( true );
//		painter.clearNodeCount();
		QRect dirty = evt->rect();
		m_pGlobe->paintGlobe(&painter,dirty);
	
		painter.drawPixmap(10, m_pCanvasImage->height()-40,
		m_mapscale.drawScaleBarPixmap( m_pGlobe->getRadius(),m_pCanvasImage-> width()/2 - 20));

		painter.drawPixmap( m_pCanvasImage->width()-60, 10,
		m_windrose.drawWindRosePixmap( m_pCanvasImage->width(), m_pCanvasImage->height(), m_pGlobe->northPoleY() ) );

		m_crosshair.paintCrossHair( &painter, m_pCanvasImage->width(), m_pCanvasImage->height() );

		m_pMeasureTool->paintMeasurePoints( &painter, m_pCanvasImage->width()/2, m_pCanvasImage->height()/2, radius, m_pGlobe->getPlanetAxis(), true );
//		qDebug() << "Nodes: " << painter.nodeCount();
//	}
/*
	else
	{
		// Draw cached pixmap to widget
		QPainter pixmapPainter(this);
		QRect rect(0, 0, width(), height());
		pixmapPainter.drawImage(rect, m_pCanvasImage, rect);
	}
*/
//		qDebug() << "PaintEvent: " << timer.elapsed();
}

void KAtlasView::goHome(){
//	m_pGlobe->rotateTo(0, 0);
	m_pGlobe->rotateTo(54.8, -9.4);
	zoomView(1050); // default 1050

	update(); // not obsolete in case the zoomlevel stays unaltered
}

float KAtlasView::getMoveStep(){
	if ( m_pGlobe->getRadius() < sqrt(width()*width() + height()*height()))
		return 0.1f;
	else
		return atanf((float)width()/(float)(2 * m_pGlobe->getRadius())) * 0.2f;
}

int KAtlasView::fromLogScale(int zoom){
	zoom = (int) pow(M_E, ((float)zoom/200));
//	zoom = (int) pow(2, ((float)zoom/200));
	return zoom;
}

int KAtlasView::toLogScale(int zoom){
	zoom = (int)(200.0f*logf((float)zoom));
	return zoom;
}

#include "katlasview.moc"
