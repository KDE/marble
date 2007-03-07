//
// C++ Interface: gpmapscale
//
// Description: KAtlasWindRose 

// KAtlasWindRose paints a windrose on a pixmap
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution

#ifndef KATLASWINDROSE_H
#define KATLASWINDROSE_H

#include <QtCore/QObject>
#include <QtGui/QPixmap>
#include <QtGui/QFont>

/**
@author Torsten Rahn
*/

class QSvgRenderer;

class KAtlasWindRose : public QObject {

Q_OBJECT

public:
	KAtlasWindRose(QObject *parent = 0);

	QPixmap& drawWindRosePixmap(int, int, int );

protected:

	QSvgRenderer* m_svgobj;
	QPixmap m_pixmap;
	int m_width;

	QFont m_font;
	int m_fontwidth, m_fontheight;
	int m_polarity;
};

#endif // KATLASWINDROSE_H
