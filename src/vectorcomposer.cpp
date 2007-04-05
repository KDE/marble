#include "vectorcomposer.h"

#include <QtGui/QColor>
#include <QtCore/QDebug>
#include <cmath>

#include "GeoPolygon.h"
#include "clippainter.h"
#include "vectormap.h"
#include "katlasdirs.h"

	const float eps = .00001f ;
	const float rad2int = 21600.0 / M_PI;

VectorComposer::VectorComposer(){

        pcoast = new PntMap();
	pcoast->load(KAtlasDirs::path("mwdbii/PCOAST.PNT"));

        pisland = new PntMap();
	pisland->load(KAtlasDirs::path("mwdbii/PISLAND.PNT"));
	plake = new PntMap();
	plake->load(KAtlasDirs::path("mwdbii/PLAKE.PNT"));
	pglacier = new PntMap();
	pglacier->load(KAtlasDirs::path("mwdbii/PGLACIER.PNT"));
	priver = new PntMap();
	priver->load(KAtlasDirs::path("mwdbii/RIVER.PNT"));

	pborder = new PntMap();
	pborder->load(KAtlasDirs::path("mwdbii/PDIFFBORDER.PNT"));

	pusa = new PntMap();
	pusa->load(KAtlasDirs::path("mwdbii/PUSA48.DIFF.PNT"));

        vectormap = new VectorMap();
	m_areapen = QPen(Qt::NoPen);
	m_riverpen.setStyle(Qt::SolidLine);
	m_riverpen.setColor(QColor(0,0,255));
	m_areabrush = QBrush(QColor(255,0,0));
	m_lakebrush = QBrush(QColor(0,0,0));
	m_borderpen.setStyle(Qt::SolidLine);
	m_borderpen.setColor(QColor(0,255,0));
}

// void VectorComposer::drawMap(QImage* origimg, const int& radius, Quaternion& rotAxis){
void VectorComposer::drawTextureMap(QPaintDevice * origimg, const int& radius, Quaternion& rotAxis){

//	vectormap -> clearNodeCount();

// Coastlines
	vectormap -> setzBoundingBoxLimit(0.4); 
	vectormap -> setzPointLimit(0.6); 

	vectormap -> createFromPntMap(pcoast,radius,rotAxis);
	vectormap -> setPen(m_areapen);
	vectormap -> setBrush(m_areabrush);
	vectormap -> drawMap(origimg, false);
// Islands
	vectormap -> setzBoundingBoxLimit(0.8); 
	vectormap -> setzPointLimit(0.9); 

	vectormap -> createFromPntMap(pisland,radius,rotAxis);
	vectormap -> setPen(m_areapen);
	vectormap -> setBrush(m_areabrush);
	vectormap -> drawMap(origimg, false);

// Glacier

	vectormap -> setzBoundingBoxLimit(0.8); 
	vectormap -> setzPointLimit(0.9); 

	vectormap -> createFromPntMap(pglacier,radius,rotAxis);
	vectormap -> setBrush(m_lakebrush);
	vectormap -> drawMap(origimg, false);
//	qDebug() << "TextureMap calculated nodes: " << vectormap->nodeCount();
}


void VectorComposer::paintVectorMap(ClipPainter* painter, const int& radius, Quaternion& rotAxis){

//	vectormap -> clearNodeCount();

// Rivers
	vectormap -> setzBoundingBoxLimit(-1.0); 
	vectormap -> setzPointLimit(-1.0); 

	vectormap -> createFromPntMap(priver,radius,rotAxis);

	vectormap -> setPen(QColor(99,123,255));
	vectormap -> paintMap(painter, false);

// Countries

	vectormap -> setzBoundingBoxLimit(-1.0); 
	vectormap -> setzPointLimit(-1.0); 

	vectormap -> createFromPntMap(pborder,radius,rotAxis);
//	QPen borderpen(QColor(242,187,136));
	QPen borderpen(QColor(242,155,104));
//	borderpen.setStyle(Qt::DashLine);

	vectormap -> setPen(borderpen);
	vectormap -> setBrush(Qt::NoBrush);
	vectormap -> paintMap(painter, false);


// US-States
	vectormap -> setzBoundingBoxLimit(-1.0); 
	vectormap -> setzPointLimit(-1.0); 

	vectormap -> createFromPntMap(pusa,radius,rotAxis);
//	QPen statepen(QColor(242,187,136));
	QPen statepen(QColor(242,155,104));
	statepen.setStyle(Qt::DotLine);
	vectormap -> setPen(statepen);
	vectormap -> setBrush(Qt::NoBrush);
	vectormap -> paintMap(painter, false);

// Lakes
	vectormap -> setzBoundingBoxLimit(0.95); 
	vectormap -> setzPointLimit(0.98); 

	vectormap -> createFromPntMap(plake,radius,rotAxis);
	vectormap -> setPen(m_areapen);
	vectormap -> setBrush(QBrush(QColor(214,226,255)));
	vectormap -> paintMap(painter, false);

/*
// Glacier

	vectormap -> setzBoundingBoxLimit(0.8); 
	vectormap -> setzPointLimit(0.9); 

	vectormap -> createFromPntMap(pglacier,radius,rotAxis);
	vectormap -> setBrush(QColor(Qt::white));
	vectormap -> drawMap(origimg);
*/
//	qDebug() << "VectorMap calculated nodes: " << vectormap->nodeCount();
}


// void VectorComposer::resizeMap(const QImage* origimg){
void VectorComposer::resizeMap(const QPaintDevice* origimg){
//	qDebug("A");
//	int imgwidth = origimg -> width();
//	qDebug(QString("width: %1").arg(imgwidth).toLatin1());
	vectormap -> resizeMap(origimg);
//	qDebug("C");
}

