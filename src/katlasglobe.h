//
// C++ Interface: katlasglobe.h
//
// Description: KAtlasGlobe 

// KAtlasGlobe provides a globe in KAtlasView
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution

#ifndef KATLASGLOBE_H
#define KATLASGLOBE_H

#include <QAbstractListModel>
#include <QObject>
#include <QPainter>
#include <QImage>
#include <QPixmap>
#include "placecontainer.h"
#include "placemarkmodel.h"
// #include <QPainter>
#include "clippainter.h"
#include "maptheme.h"
#include "texmapper.h"
#include "gridmap.h"
#include "vectorcomposer.h"
#include "texcolorizer.h"
#include "placemarkpainter.h"
#include "quaternion.h"

class PlaceMark;
class PlaceMarkManager;

/**
@author Torsten Rahn
*/

class KAtlasGlobe  : public QObject {
public:
	KAtlasGlobe( QWidget* parent=0 );
	virtual ~KAtlasGlobe(){};
	void setCanvasImage(QImage*);

	void paintGlobe(ClipPainter*, QRect);

	void resize();

	void zoom(const int&);
	int getRadius(){ return m_radius; };
	Quaternion getRotAxis(){ return m_rotAxis; };

	void rotateTo(const uint&, const uint&, const uint&);

// This method provides a way to center on lat = +90(N) - -90(S) and lng = +180(W) - -180(E) 
	void rotateTo(const float&, const float&);

	void rotateBy(const Quaternion&);
	void rotateBy(const float&, const float&);
	QAbstractListModel* getPlaceMarkModel(){ return m_placemarkmodel; };

	void setMapTheme( const QString& );

	int northPoleY();
	int northPoleZ();

	bool screenCoordinates( const float lng, const float lat, int& x, int& y );

	bool needsUpdate() const { return !( m_radius == m_radiusUpdated && m_rotAxis == m_rotAxisUpdated ); }

	void addPlaceMarkFile( QString filename );

	QVector< PlaceMark* > whichFeatureAt( const QPoint& );
	PlaceContainer* placeContainer(){ return m_placecontainer ; }

protected:
	QWidget* m_parent;
	QImage* m_canvasimg;
	QImage* m_coastimg;

	MapTheme* m_maptheme;
	TextureColorizer* texcolorizer;
	TextureMapper* texmapper;
	VectorComposer* veccomposer;
	GridMap* gridmap;
	PlaceMarkManager* placemarkmanager;
//	PlaceComposer* placecomposer;
	PlaceMarkModel* m_placemarkmodel;
	PlaceMarkPainter* m_placemarkpainter;
	PlaceContainer* m_placecontainer;

	Quaternion m_rotAxis, m_rotAxisUpdated;
	int m_radius, m_radiusUpdated;

	bool m_justModified;
	bool m_centered;
};

#endif // KATLASGLOBE_H
