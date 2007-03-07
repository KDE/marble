//
// C++ Interface: vecmapper
//
// Description: VectorComposer 

// The VectorComposer maps the data stored as polylines onto the respective projection.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution

#ifndef VECTORCOMPOSER_H
#define VECTORCOMPOSER_H

#include <QBrush>
#include <QtGui/QImage>
#include <QtGui/QPen>
#include <QtGui/QPixmap>
#include "quaternion.h"

/**
@author Torsten Rahn
*/

class ClipPainter;
class PntMap;
class VectorMap;

class VectorComposer {
public:
	VectorComposer();
	virtual ~VectorComposer(){};

	void drawTextureMap(QPaintDevice*, const int&, Quaternion&);
	void paintVectorMap(ClipPainter*, const int&, Quaternion&);
	void resizeMap(const QPaintDevice *);

private:
	PntMap* pcoast;
	PntMap* pisland;
	PntMap* plake;
	PntMap* pglacier;
	PntMap* priver;
	PntMap* pborder;
	PntMap* pusa;

	VectorMap* vectormap;
	QPen m_areapen, m_riverpen, m_borderpen;
	QBrush m_areabrush, m_lakebrush;

	QVector<qreal> m_dashes;
};

#endif // VECTORMAP_H
