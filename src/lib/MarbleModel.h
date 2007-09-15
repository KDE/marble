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



#include "marble_export.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QObject>
#include <QtGui/QPainter>
#include <QtGui/QImage>
#include <QtGui/QPixmap>

#include "global.h"
#include "Quaternion.h"
#include "GridMap.h"
#include "PlaceMarkContainer.h"
#include "PlaceMarkPainter.h"
#include "PlaceMarkModel.h"
#include "GlobeScanlineTextureMapper.h"
#include "FlatScanlineTextureMapper.h"
#include "vectorcomposer.h"
#include "texcolorizer.h"

class ClipPainter;
class PlaceMark;
class PlaceMarkManager;
class GpsLayer;
class GpxFileModel;
class MarbleModelPrivate;
class ViewParams;
class TileCreator;


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

    void  paintGlobe(ClipPainter*, int width, int height,
                     ViewParams *viewParams,
                     bool redrawBackground, const QRect&);

    void  resize( int width, int height );

    QAbstractListModel* getPlaceMarkModel() const;

    QString mapTheme() const;

    void setMapTheme( const QString &selectedMap, QWidget *parent, Projection currentProjection );

    void addPlaceMarkFile( const QString& filename );

    QVector< PlaceMark* > whichFeatureAt( const QPoint& );

    PlaceMarkContainer *placeMarkContainer() const;
    PlaceMarkPainter   *placeMarkPainter()   const;
    VectorComposer     *vectorComposer()     const;
    TextureColorizer   *textureColorizer()   const;

    AbstractScanlineTextureMapper  *textureMapper() const;

    GpsLayer           *gpsLayer()           const;
    GpxFileModel       *gpxFileModel()       const;

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

    /**
     * @brief Start the model's timer polling
     *
     * this is default behaviour so does not need to be started unless
     * it was previously stopped
     * @param time the amount of milliseconds between each poll
     */
    void startPolling( int time = 1000 );

    /**
     * @brief stop the model from polling
     */
    void stopPolling();

 Q_SIGNALS:
    void creatingTilesStart( TileCreator*, const QString& name, const QString& description );

    void themeChanged( QString );
    void modelChanged();
    void regionChanged( BoundingBox );
    void timeout();

 private Q_SLOTS:
    void notifyModelChanged();

 private:
    MarbleModelPrivate  * const d;
};


#endif // MARBLEMODEL_H
