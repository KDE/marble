#include "katlasview.h"

#include <cmath>
#include <QAbstractItemModel>
#include <QSizePolicy>
#include <QTime>
#include <QSizePolicy>
#include <QRegion>

#include "clippainter.h"
#include "katlasviewinputhandler.h"
#include "quaternion.h"
#include "texcolorizer.h"

KAtlasView::KAtlasView(QWidget *parent)
    : QWidget(parent)
{

	setFocusPolicy(Qt::WheelFocus);
        setFocus(Qt::OtherFocusReason);
	globe = new KAtlasGlobe( this );

	QPalette p = palette();
	p.setColor(QPalette::Window,Qt::black);
	setPalette(p);

	setBackgroundRole(QPalette::Window);
	setAutoFillBackground(true);

//	setAttribute(Qt::WA_NoSystemBackground);

	inputhandler = new KAtlasViewInputHandler(this, globe);
	installEventFilter(inputhandler);
	setMouseTracking(true);

	canvasimg = QImage(parent->width(),parent->height(),QImage::Format_ARGB32_Premultiplied);
	globe->setCanvasImage( &canvasimg );

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

	zoom = fromLogScale(zoom);

//	if ( zoom < minimumzoom ) zoom = minimumzoom;
	
	globe->zoom(zoom);

	repaint();

	setActiveRegion();
}

void KAtlasView::zoomViewBy(int zoomstep){
	// prevent infinite loops

	int zoom = globe->getRadius();
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
	globe->rotateBy(phi, theta);

	repaint();
}

void KAtlasView::centerOn(const float& phi, const float& theta){
	globe->rotateTo(phi, theta);

	repaint();
}

void KAtlasView::centerOn(const QModelIndex& index){
	int row = index.row();
//	qDebug() << row;
	QAbstractItemModel* model = globe->getPlaceMarkModel();
	QModelIndex mlat = model->index(row,4,QModelIndex());
	float lat = (float)(model->data(mlat,Qt::DisplayRole).toDouble());
	QModelIndex mlng = model->index(row,5,QModelIndex());
	float lng = (float)(model->data(mlng,Qt::DisplayRole).toDouble());

	centerOn(lat, lng);
	globe->setCenterOn(row);
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
	canvasimg = QImage(width(),height(),QImage::Format_ARGB32_Premultiplied);
	globe->resize();

	repaint();
}

bool KAtlasView::getGlobeSphericals(int x, int y, float& alpha, float& beta){

	int radius = globe->getRadius(); 
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
		qpos.rotateAroundAxis(globe->getRotAxis());
		qpos.getSpherical( alpha, beta );

		return true;
	}
	else {
		return false;
	}
}

void KAtlasView::setActiveRegion(){
	int zoom = globe->getRadius(); 

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
//	Debugging Active Region
//	painter.setClipRegion(activeRegion);

//	if(globe->needsUpdate() || canvasimg.isNull() || canvasimg.size() != size())
//	{
		int radius = globe->getRadius();
		bool clip = (radius > canvasimg.width()/2 || radius > canvasimg.height()/2) ? true : false;
		ClipPainter painter( this, clip);

		QRect dirty = evt->rect();
		globe->paintGlobe(&painter,dirty);
	
//		painter.translate(10,canvasimg->height()-40);
		painter.drawPixmap(10, canvasimg.height()-40,
		m_mapscale.drawScaleBarPixmap( globe->getRadius(),canvasimg.width()/2 - 20));

//		painter.translate(canvasimg->width()-100,40);

		painter.drawPixmap( canvasimg.width()-60, 10,
		m_windrose.drawWindRosePixmap( canvasimg.width(), canvasimg.height(), globe->northPoleY() ) );
//	}
/*
	else
	{
		// Draw cached pixmap to widget
		QPainter pixmapPainter(this);
		QRect rect(0, 0, width(), height());
		pixmapPainter.drawImage(rect, canvasimg, rect);
	}
*/
}

void KAtlasView::goHome(){
//	globe->rotateTo(0, 0);
	globe->rotateTo(54.8, -9.4);
	zoomView(1100);

	update(); // not obsolete in case the zoomlevel stays unaltered
}

float KAtlasView::getMoveStep(){
	if ( globe->getRadius() < sqrt(width()*width() + height()*height()))
		return 0.1;
	else
		return atan((float)width()/(float)(2 * globe->getRadius())) * 0.2;
}

int KAtlasView::fromLogScale(int zoom){
	zoom = (int) pow(M_E, ((float)zoom/200));
//	zoom = (int) pow(2, ((float)zoom/200));
	return zoom;
}

int KAtlasView::toLogScale(int zoom){
	zoom = (int)(200*log((float)zoom));
	return zoom;
}

