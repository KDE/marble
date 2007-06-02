//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2005-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

//
// MarbleModel is the data store and index class for the MarbleWidget.
//


#ifndef MARBLEMODEL_H
#define MARBLEMODEL_H

#include "marble_export.h"

/** @file
 * This file contains the headers for MarbleModel
 * 
 * @author Torsten Rahn <tackat@kde.org>
 * @author Inge Wallin  <inge@lysator.liu.se>
 */


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
 * @short The data model (not based on QAbstractModel) for a MarbleWidget.
 *
 * This class provides a data storage and indexer that can be
 * displayed in a MarbleWidget.  It contains 3 different datatypes:
 * <b>tiles</b> which provide the background, <b>vectors</b> which
 * provide things like country borders and coastlines and
 * <b>placemarks</b> which can show points of interest, such as
 * cities, mountain tops or the poles.
 *
 * The <b>tiles</b> provide the background of the image and can be for
 * instance height and depth fields, magnetic strength, topographic
 * data or anything else that is area based.
 *
 * The <b>vectors</b> provide things like country borders and
 * coastlines.  They are stored in separate files and can be added or
 * removed at anytime.
 *
 * The <b>placemarks</b> contain points of interest, such as cities,
 * mountain tops or the poles. These are sorted by size (for cities)
 * and category (capitals, other important cities, less important
 * cities, etc) and are displayed with different color or shape like
 * square or round.
 *
 * @see MarbleWidget
 */

class MARBLE_EXPORT MarbleModel  : public QObject
{
    Q_OBJECT

 public:
    MarbleModel( QWidget* parent=0 );
    virtual ~MarbleModel();

    void  setCanvasImage(QImage*);

    void  paintGlobe(ClipPainter*, const QRect&);

    void  resize();

    int   radius(){ return m_radius; };
    void  setRadius(const int&);

    Quaternion  getPlanetAxis(){ return m_planetAxis; };


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

    bool needsUpdate() const { return !( m_radius == m_radiusUpdated && m_planetAxis == m_planetAxisUpdated ); }

    void addPlaceMarkFile( const QString& filename );

    QVector< PlaceMark* > whichFeatureAt( const QPoint& );

    PlaceContainer* placeContainer(){ return m_placecontainer ; }

 signals:
    void creatingTilesStart( const QString& name, const QString& description );
    void creatingTilesProgress( int progress );

    void themeChanged();

 protected:
    QWidget  *m_parent;
    QImage   *m_canvasimg;
    QImage   *m_coastimg;

    // View and paint stuff
    MapTheme          *m_maptheme;
    TextureColorizer  *m_texcolorizer;
    TextureMapper     *m_texmapper;
    VectorComposer    *m_veccomposer;
    GridMap           *m_gridmap;

    // Places on the map
    PlaceMarkManager  *m_placemarkmanager;
    PlaceMarkModel    *m_placemarkmodel;
    PlaceMarkPainter  *m_placemarkpainter;
    PlaceContainer    *m_placecontainer;

    Quaternion         m_planetAxis;
    Quaternion         m_planetAxisUpdated;
    int                m_radius;
    int                m_radiusUpdated;

    bool  m_justModified;
    bool  m_centered;
};


#endif // MARBLEMODEL_H
