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

class QItemSelectionModel;

class AbstractScanlineTextureMapper;
class BoundingBox;
class ClipPainter;
class FileViewModel;
class GeoDataDocument;
class GeoSceneDocument;
class GpsLayer;
class GpxFileModel;
class HttpDownloadManager;
class MarbleModelPrivate;
class PlaceMarkLayout;
class SunLocator;
class TextureColorizer;
class TileCreator;
class VectorComposer;
class ViewParams;
class TextureTile;
class MergedLayerDecorator;


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
    friend class MarbleModelPrivate;

    Q_OBJECT

 public:
    /**
     * @brief  Construct a new MarbleModel.
     * @param parent the parent widget
     */
    MarbleModel( QObject *parent );
    virtual ~MarbleModel();

    /**
     * @brief   Paint the model into the view
     * @param painter  the QPainter used to paint the view
     * @param width    the width of the widget
     * @param height   the height of the widget
     * @param viewParams  the view parameters controlling the paint process
     * @param redrawBackground  a boolean controlling if the background should be redrawn in addition to the globe itself
     * @param dirtyRect  the rectangle of the widget that needs redrawing.
     *
     * The model has the responsibility to actually paint into the
     * MarbleWidget.  This function is called by MarbleWidget when it
     * receives a paintEvent and should repaint whole or part of the
     * widget's contents based on the parameters.
     *
     * NOTE: This function will probably move to MarbleWidget in KDE
     * 4.1, making the MarbleModel/MarbleWidget pair truly follow the
     * Model/View paradigm.
     */
    void  paintGlobe(ClipPainter *painter, int width, int height,
                     ViewParams *viewParams,
                     bool redrawBackground, const QRect& dirtyRect);

    /**
     * @brief Return the list of PlaceMarks as a QAbstractItemModel *
     * @return a list of all PlaceMarks in the MarbleModel.
     */
    QAbstractItemModel*  placeMarkModel() const;
    QItemSelectionModel* placeMarkSelectionModel() const;

    /**
     * @brief Return the name of the current map theme.
     * @return the identifier of the current MapTheme.
     * To ensure that a unique identifier is being used the theme does NOT 
     * get represented by its name but the by relative location of the file 
     * that specifies the theme:
     *
     * Example: 
     *    maptheme = "earth/bluemarble/bluemarble.dgml"
     */
    QString mapTheme() const;

    GeoSceneDocument *mapThemeObject() const;

    /**
     * @brief Set a new map theme to use.
     * @param selectedMap  the identifier of the selected map theme
     * @param parent       the parent widget
     * @param currentProjection  the current projection
     *
     * This function sets the map theme, i.e. combination of tile set
     * and color scheme to use.  If the map theme is not previously
     * used, some basic tiles are created and a progress dialog is
     * shown.  This is what the parent parameter is used for.
     *
     * NOTE: Both the parent and currentProjection parameters will
     *       disappear soon.
     *
     * The ID of the new maptheme. To ensure that a unique 
     * identifier is being used the theme does NOT get represented by its 
     * name but the by relative location of the file that specifies the theme:
     *
     * Example: 
     *    maptheme = "earth/bluemarble/bluemarble.dgml" 
     */
    void setMapTheme( const QString &selectedMap,
		      //QWidget *parent,
		      QObject *parent, /* FIXME */
		      Projection currentProjection );

    /**
     * @brief Return the minimum zoom for the current map theme.
     */
    int minimumZoom() const;

    /**
     * @brief Return the maximum zoom for the current map theme.
     */
    int maximumZoom() const;

    /**
     * @brief Return the downloadmanager to load missing tiles
     * @return the HttpDownloadManager instance.
     */

    HttpDownloadManager* downloadManager() const;

   /**
     * @brief Set the downloadmanager to load missing tiles
     * @param downloadManager the HttpDownloadManager instance
     *
     * This function sets the downloadmanager to load tiles that
     * are missing locally. The desctruction of the instance
     * will be handled by MarbleModel.
     */
    void setDownloadManager( HttpDownloadManager *downloadManager );

    void addPlaceMarkFile( const QString& filename );

    QVector<QPersistentModelIndex> whichFeatureAt( const QPoint& ) const;

    PlaceMarkLayout    *placeMarkLayout()   const;
    VectorComposer     *vectorComposer()     const;
    TextureColorizer   *textureColorizer()   const;

    AbstractScanlineTextureMapper  *textureMapper() const;

    GpsLayer           *gpsLayer()           const;
    GpxFileModel       *gpxFileModel()       const;
    FileViewModel      *fileViewModel()   const;

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
     * @param time the amount of milliseconds between each poll
     *
     * This is default behaviour so does not need to be started unless
     * it was previously stopped
     */
    void startPolling( int time = 1000 );

    /**
     * @brief stop the model from polling
     */
    void stopPolling();

    SunLocator* sunLocator() const;
    MergedLayerDecorator* layerDecorator() const;

    /**
     * @brief  Returns the limit of the volatile (in RAM) tile cache.
     * @return the cache limit in kilobytes
     */
    quint64 volatileTileCacheLimit() const;
    

 public Q_SLOTS:
    /**
     * @brief Set the limit of the volatile (in RAM) tile cache.
     * @param kilobytes The limit in kilobytes.
     */
    void setVolatileTileCacheLimit( quint64 kiloBytes );

//  private Q_SLOTS:
    void paintTile(TextureTile* tile, int x, int y, int level, const QString& theme, bool requestTileUpdate);

    /**
     * @brief Update the model
     */
    void update();

 Q_SIGNALS:

    /**
     * @brief Signal that the MarbleModel has started to create a new set of tiles.
     * @param 
     * @see  zoomView()
     */
    void creatingTilesStart( TileCreator*, const QString& name, const QString& description );


    /**
     * @brief Signal that the map theme has changed, and to which theme.
     * @param name the identifier of the new map theme.
     * @see  mapTheme
     * @see  setMapTheme
     */
    void themeChanged( QString mapTheme );
    /**
     * @brief Signal that the MarbleModel has changed in general
     */
    void modelChanged();
    /**
     * @brief Signal that a region of the earth has changed
     * @param rect the region that changed.
     *
     * This is currently used only for gpx files, but will likely be
     * used for more things in the future.
     */
    void regionChanged( BoundingBox &rect );

    /**
     * @brief Signal that a timer has gone off.
     *
     * This is currently used only for GPS things right now, but will
     * likely be used for more things in the future.
     */
    void timeout();

 private:
    MarbleModelPrivate  * const d;

    Q_PRIVATE_SLOT( d, void notifyModelChanged() )
    Q_PRIVATE_SLOT( d, void geoDataDocumentLoaded( GeoDataDocument& ) )
    
    SunLocator* m_sunLocator;
    MergedLayerDecorator* m_layerDecorator;
};


#endif // MARBLEMODEL_H
