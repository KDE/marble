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

#include <QFont>
#include <QPixmap>
#include <QVector>
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
	void paintPlaceMark(QPainter*, int, int, const QAbstractItemModel*, int);
	void paintPlaceFolder(QPainter*, int, int, int, const PlaceContainer*, Quaternion );

public slots:


private:
	QFont m_font;
	int m_fontheight, m_fontascent;
	QVector<QPixmap> m_citysymbol;
	QPixmap m_empty;
};

#endif // PLACEMARKPAINTER_H
