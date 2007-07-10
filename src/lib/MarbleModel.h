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


/** @file
 * This file contains the headers for MarbleModel
 * 
 * @author Torsten Rahn <tackat@kde.org>
 * @author Inge Wallin  <inge@lysator.liu.se>
 */

// #define FLAT_PROJ
#include "marble_export.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QObject>
#include <QtGui/QPainter>
#include <QtGui/QImage>
#include <QtGui/QPixmap>

#include "Quaternion.h"
#include "GridMap.h"
#include "PlaceMarkContainer.h"
#include "placemarkpainter.h"
#include "placemarkmodel.h"
#include "clippainter.h"
#include "maptheme.h"
#include "GlobeScanlineTextureMapper.h"
#include "FlatScanlineTextureMapper.h"
#include "vectorcomposer.h"
#include "texcolorizer.h"


class PlaceMark;
class PlaceMarkManager;
class GpsLayer;
class MarbleModelPrivate;
class ViewParams;


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

class MARBLE_EXPORT MarbleModel : public QObject
{
    Q_OBJECT

 public:
    /**
     * @brief  Construct a new MarbleModel.
     * @param view  The widget that is the view for this Model. 
     */
    MarbleModel( QWidget *parent );
    virtual ~MarbleModel();

    void  paintGlobe(ClipPainter*, ViewParams *viewParams, const QRect&);

    void  resize( QImage *canvasImage );

    int   radius() const;
    void  setRadius(const int radius);

    Quaternion  getPlanetAxis() const;


    // This method provides a way to center on lat = +90(N) - -90(S) and lng = +180(W) - -180(E) 
    void  rotateTo(const double&, const double&);
    void  rotateTo(const uint&, const uint&, const uint&);
    void  rotateTo(const Quaternion&);

    void rotateBy(const Quaternion&);
    void rotateBy(const double&, const double&);

    double  centerLatitude()  const;
    double  centerLongitude() const;

    QAbstractListModel* getPlaceMarkModel() const;

    void setMapTheme( const QString &selectedMap, QWidget *parent );

    int northPoleY();
    int northPoleZ();

    bool screenCoordinates( const double lng, const double lat, int& x, int& y );

    bool needsUpdate() const;
    void setNeedsUpdate();

    void addPlaceMarkFile( const QString& filename );

    QVector< PlaceMark* > whichFeatureAt( const QPoint& );

    PlaceMarkContainer *placeMarkContainer() const;
    PlaceMarkPainter   *placeMarkPainter()   const;
    VectorComposer     *vectorComposer()     const;
    TextureColorizer   *textureColorizer()   const;
#ifndef FLAT_PROJ
    GlobeScanlineTextureMapper
#else
    FlatScanlineTextureMapper
#endif
        *textureMapper()      const;
    GpsLayer           *gpsLayer()           const;

    /**
     * @brief  Return whether the coordinate grid is visible.
     * @return The coordinate grid visibility.
     */
    bool  showGrid() const;

    /**
     * @brief  Set whether the coordinate grid overlay is visible
     * @param  visible  visibility of the coordinate grid
     */
    void setShowGrid( bool visible );

    /**
     * @brief  Return whether the place marks are visible.
     * @return The place mark visibility.
     */
    bool  showPlaceMarks() const;

    /**
     * @brief  Set whether the place mark overlay is visible
     * @param  visible  visibility of the place marks
     */
    void setShowPlaceMarks( bool visible );

    /**
     * @brief  Return whether the elevation model is visible.
     * @return The elevation model visibility.
     */
    bool  showElevationModel() const;

    /**
     * @brief  Set whether the elevation model is visible
     * @param  visible  visibility of the elevation model
     */
    void setShowElevationModel( bool visible );
    
    /**
     * @brief Return whether the Gps Data is visible.
     * @return The Gps Data visibility.
     */
    bool showGps() const;
    
    /**
     * @brief Set whether the Gps Data is visible.
     * @param visible visibility of the Gps Data.
     */
    void setShowGps( bool visible );

 Q_SIGNALS:
    void creatingTilesStart( const QString& name, const QString& description );
    void creatingTilesProgress( int progress );

    void modelChanged();
    void timeout();

 private Q_SLOTS:
    void notifyModelChanged();

 private:
    MarbleModelPrivate  * const d; 
};


#endif // MARBLEMODEL_H
