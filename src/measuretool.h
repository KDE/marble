//
// C++ Interface: measuretool
//
// Description: MeasureTool 

// MeasureTool enables Marble to set and display measure points
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution

#ifndef MEASURETOOL_H
#define MEASURETOOL_H

#include <QtGui/QFont>
#include <QtCore/QVector>

#include "GeoPoint.h"
#include "placemark.h"
#include "clippainter.h"

/**
@author Torsten Rahn
*/

class MeasureTool : public QObject {

Q_OBJECT

public:
	MeasureTool(QObject *parent = 0);
	void paintMeasurePoints(ClipPainter*, int, int, int, Quaternion, bool );
	void setLineColor(QColor linecolor){ m_linecolor = linecolor;}
	void paintTotalDistanceLabel(ClipPainter*, int, int, float );

public slots:
	void addMeasurePoint( float lng, float lat ){ m_pMeasurePointList << new GeoPoint( lng, lat );}
	void removeMeasurePoints(){ m_pMeasurePointList.clear(); }

protected:
	float m_totalDistance;

	QFont m_font_regular;
	int m_fontheight, m_fontascent;

	QColor m_linecolor;

	QVector<GeoPoint*> m_pMeasurePointList;

	QPen m_pen;

	QPixmap m_empty;

	bool testbug(); 
	bool m_useworkaround;

	void paintMark( ClipPainter* painter, int x, int y );
	void drawDistancePath( ClipPainter* painter, Quaternion, Quaternion, int imgrx, int imgry, int radius, bool antialiasing );
};

#endif // MEASURETOOL_H
