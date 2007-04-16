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

#include <QtCore/QAbstractListModel>
#include <QtCore/QObject>
#include <QtGui/QPainter>
#include <QtGui/QImage>
#include <QtGui/QPixmap>

#include "Quaternion.h"
#include "GridMap.h"
#include "placecontainer.h"
#include "placemarkmodel.h"
#include "clippainter.h"
#include "maptheme.h"
#include "TextureMapper.h"
#include "vectorcomposer.h"
#include "texcolorizer.h"
#include "placemarkpainter.h"

class PlaceMark;
class PlaceMarkManager;

/**
@author Torsten Rahn
*/

class KAtlasGlobe  : public QObject
{
    Q_OBJECT

 public:
    KAtlasGlobe( QWidget* parent=0 );
    virtual ~KAtlasGlobe();

    void  setCanvasImage(QImage*);

    void  paintGlobe(ClipPainter*, QRect);

    void  resize();

    int   radius(){ return m_radius; };
    void  setRadius(const int&);

    Quaternion  getPlanetAxis(){ return m_pPlanetAxis; };


    // This method provides a way to center on lat = +90(N) - -90(S) and lng = +180(W) - -180(E) 
    void  rotateTo(const float&, const float&);
    void  rotateTo(const uint&, const uint&, const uint&);

    void rotateBy(const Quaternion&);
    void rotateBy(const float&, const float&);

    QAbstractListModel* getPlaceMarkModel(){ return m_placemarkmodel; };

    void setMapTheme( const QString& );

    int northPoleY();
    int northPoleZ();

    bool screenCoordinates( const float lng, const float lat, int& x, int& y );

    bool needsUpdate() const { return !( m_radius == m_radiusUpdated && m_pPlanetAxis == m_pPlanetAxisUpdated ); }

    void addPlaceMarkFile( QString filename );

    QVector< PlaceMark* > whichFeatureAt( const QPoint& );

    PlaceContainer* placeContainer(){ return m_placecontainer ; }

 signals:
    void creatingTilesStart( QString name, QString description );
    void creatingTilesProgress( int progress );

    void themeChanged();

 protected:
    QWidget  *m_parent;
    QImage   *m_canvasimg;
    QImage   *m_coastimg;

    // View and paint stuff
    MapTheme          *m_maptheme;
    TextureColorizer  *texcolorizer;
    TextureMapper     *texmapper;
    VectorComposer    *veccomposer;
    GridMap           *gridmap;

    PlaceMarkManager  *placemarkmanager;
    PlaceMarkModel    *m_placemarkmodel;
    PlaceMarkPainter  *m_placemarkpainter;
    PlaceContainer    *m_placecontainer;

    Quaternion         m_pPlanetAxis;
    Quaternion         m_pPlanetAxisUpdated;
    int                m_radius;
    int                m_radiusUpdated;

    bool  m_justModified;
    bool  m_centered;
};


#endif // KATLASGLOBE_H
