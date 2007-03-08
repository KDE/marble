//
// C++ Interface: gpmapscale
//
// Description: KAtlasMapScale 

// KAtlasMapScale resembles the scale in terms of value and visualisation
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution

#ifndef PLACEMARKPAINTER_H
#define PLACEMARKPAINTER_H

#include <QtGui/QFont>
#include <QtGui/QPainterPath>
#include <QtGui/QPixmap>
#include <QtCore/QRect>
#include <QtCore/QVector>
#include "placecontainer.h"
#include "placemark.h"
#include "quaternion.h"

/**
@author Torsten Rahn
*/

class QAbstractItemModel;
class QPainter;
class PlaceContainer;

class PlaceMarkPainter : public QObject {

Q_OBJECT

public:
	PlaceMarkPainter(QObject *parent = 0);
	void paintPlaceFolder(QPainter*, int, int, int, const PlaceContainer*, Quaternion );
	void setLabelColor(QColor labelcolor){ m_labelcolor = labelcolor;}
	QVector<PlaceMark*> whichPlaceMarkAt( const QPoint& );

public slots:


protected:

	QFont m_font_regular, m_font_regular_italics, m_font_regular_underline, m_font_mountain;

	PlaceContainer m_visibleplacemarks;

	QColor m_labelcolor;
	int m_fontheight, m_fontascent;
	int m_labelareaheight;

//	QVector < QPixmap > m_citysymbol;
	QVector < int > m_weightfilter;
	QPixmap m_empty;
	float m_widthscale;

	bool testbug(); 
	bool m_useworkaround;
};

#endif // PLACEMARKPAINTER_H
