//
// C++ Interface: gpmapscale
//
// Description: KAtlasMapScale 

// KAtlasMapScale resembles the scale in terms of value and visualisation
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution

#ifndef KATLASWINDROSE_H
#define KATLASWINDROSE_H

#include <QObject>
#include <QPixmap>
#include <QFont>

/**
@author Torsten Rahn
*/

class QSvgRenderer;

class KAtlasWindRose : public QObject {

Q_OBJECT

public:
	KAtlasWindRose(QObject *parent = 0);

//	void setFont(QFont font){ m_font = font; }

	QPixmap& drawWindRosePixmap(int, int, int );

public slots:

//	void setScaleBarWidth(int scalebarwidth){ m_scalebarwidth = scalebarwidth; }

protected:

	QSvgRenderer* m_svgobj;
	QPixmap m_pixmap;
	int m_width;

	QFont m_font;
	int m_fontwidth, m_fontheight;
	int m_polarity;
};

#endif // KATLASWINDROSE_H
