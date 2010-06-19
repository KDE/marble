//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2005-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

//
// MarbleModel is the data store and index class for the MarbleWidget.
//

#ifndef MARBLE_MARBLEMODEL_H
#define MARBLE_MARBLEMODEL_H


/** @file
 * This file contains the headers for MarbleModel
 *
 * @author Torsten Rahn <tackat@kde.org>
 * @author Inge Wallin  <inge@lysator.liu.se>
 */



#include "marble_export.h"

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QModelIndex>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtGui/QRegion>

#include "global.h"

class QItemSelectionModel;
class QAbstractItemModel;
class QPoint;
class QRect;

namespace Marble
{

class AbstractDataPlugin;
class AbstractDataPluginItem;
class AbstractScanlineTextureMapper;
class GeoPainter;
class TileCoordsPyramid;
class FileViewModel;
class GpxFileModel;
class PositionTracking;
class HttpDownloadManager;
class MarbleModelPrivate;
class PlacemarkLayout;
class ExtDateTime;
class SunLocator;
class TextureColorizer;
class TileCreator;
class VectorComposer;
class ViewParams;
class StackedTile;
class MergedLayerDecorator;
class RenderPlugin;
class PluginManager;
class AbstractFloatItem;
class GeoDataDocument;
class GeoSceneDocument;
class GeoSceneTexture;
class Planet;
class LayerInterface;

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
    explicit MarbleModel( QObject *parent );
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
    void  paintGlobe(GeoPainter *painter, int width, int height,
                     ViewParams *viewParams,
                     bool redrawBackground, const QRect& dirtyRect);

    /**
     * @brief Return the list of Placemarks as a QAbstractItemModel *
     * @return a list of all Placemarks in the MarbleModel.
     */
    QAbstractItemModel*  placemarkModel() const;
    QItemSelectionModel* placemarkSelectionModel() const;

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
    QString mapThemeId() const;

    GeoSceneDocument *mapTheme() const;

    /**
     * @brief Set a new map theme to use.
     * @param selectedMap  the identifier of the selected map theme
     * @param currentProjection  the current projection
     *
     * This function sets the map theme, i.e. combination of tile set
     * and color scheme to use.  If the map theme is not previously
     * used, some basic tiles are created and a progress dialog is
     * shown.
     *
     * NOTE: The currentProjection parameters will
     *       disappear soon.
     *
     * The ID of the new maptheme. To ensure that a unique 
     * identifier is being used the theme does NOT get represented by its 
     * name but the by relative location of the file that specifies the theme:
     *
     * Example: 
     *    maptheme = "earth/bluemarble/bluemarble.dgml" 
     */
    void setMapTheme( GeoSceneDocument* mapTheme,
		      Projection currentProjection );

    /**
     * @brief  Set the Projection used for the map
     * @param  projection projection type (e.g. Spherical, Equirectangular, Mercator)
     */
    void  setupTextureMapper( Projection projection );

    /**
     * @brief  Setup the Vector Composer
     */
    void  setupVectorComposer();
    
    /**
     * @brief Return the downloadmanager to load missing tiles
     * @return the HttpDownloadManager instance.
     */

    HttpDownloadManager* downloadManager() const;

    void openGpxFile( const QString& filename );
    void addPlacemarkFile( const QString& filename );
    void addPlacemarkData( const QString& data, const QString& key = "data" );
    void removePlacemarkKey( const QString& key );

    QVector<QModelIndex> whichFeatureAt( const QPoint& ) const;

    PlacemarkLayout    *placemarkLayout()   const;
    VectorComposer     *vectorComposer()     const;
    /**
     * @brief Returns the map's TextureColorizer
     * @warning The TextureColorizer pointer may be null since it is only
     * initialized if the map theme requires it. Check for this possibility.
     * @return A pointer to the TextureColorizer
     */
    TextureColorizer   *textureColorizer()   const;

    AbstractScanlineTextureMapper  *textureMapper() const;

    GpxFileModel       *gpxFileModel()       const;
    FileViewModel      *fileViewModel()   const;
    PositionTracking   *positionTracking() const;

    qreal                 planetRadius()   const;
    QString               planetName()     const;

    ExtDateTime*          dateTime()       const;
    SunLocator*           sunLocator()     const;
    MergedLayerDecorator* layerDecorator() const;

    /**
     * @brief  Returns the limit of the volatile (in RAM) tile cache.
     * @return the cache limit in kilobytes
     */
    quint64 volatileTileCacheLimit() const;

    PluginManager* pluginManager() const;

    /**
     * @brief Returns a list of all RenderPlugins in the model, this includes float items
     * @return the list of RenderPlugins
     */
    QList<RenderPlugin *>      renderPlugins() const;
    /**
     * @brief Returns a list of all FloatItems in the model
     * @return the list of the floatItems
     */
    QList<AbstractFloatItem *> floatItems()    const;
    /**
     * @brief Returns a list of all DataPlugins on the layer
     * @return the list of DataPlugins
     */
    QList<AbstractDataPlugin *> dataPlugins()  const;
    
    /**
     * @brief Returns all widgets of dataPlugins on the position curpos 
     */
    QList<AbstractDataPluginItem *> whichItemAt( const QPoint& curpos ) const;

    /**
     * @brief Add a layer to be included in rendering.
     */
    void addLayer( LayerInterface *layer );

    /**
     * @brief Remove a layer from being included in rendering.
     */
    void removeLayer( LayerInterface *layer );

    /**
     * @brief Returns the planet object for the current map.
     * @return the planet object for the current map
     */
    Planet* planet() const;

    /**
     * @brief Return the current tile zoom level. For example for OpenStreetMap
     *        possible values are 1..18, for BlueMarble 0..6.
     */
    int tileZoomLevel() const;

    void reloadMap() const;
    void downloadRegion( QString const & mapThemeId, TileCoordsPyramid const & ) const;

 public Q_SLOTS:
    void clearVolatileTileCache();
    /**
     * @brief Set the limit of the volatile (in RAM) tile cache.
     * @param kilobytes The limit in kilobytes.
     */
    void setVolatileTileCacheLimit( quint64 kiloBytes );

    void clearPersistentTileCache();

//  private Q_SLOTS:
    void paintTile( StackedTile* tile, GeoSceneTexture *textureLayer );

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
    void tileLevelChanged( int newTileLevel );

    /**
     * @brief Signal that the map theme has changed, and to which theme.
     * @param mapTheme the identifier of the new map theme.
     * @see  mapTheme
     * @see  setMapTheme
     */
    void themeChanged( QString mapTheme );
    /**
     * @brief Signal that the MarbleModel has changed in general
     */
    void modelChanged();

    /**
     * This signal is emit when the settings of a plugin changed.
     */
    void pluginSettingsChanged();

    /**
     * This signal is emitted when the repaint of the view was requested.
     * If available with the @p dirtyRegion which is the region the view will change in.
     * If dirtyRegion.isEmpty() returns true, the whole viewport has to be repainted.
     */
    void repaintNeeded( QRegion dirtyRegion = QRegion() );

    /**
     * @brief Signal that a render item has been initialized
     */
    void renderPluginInitialized( RenderPlugin *renderPlugin );

 private:
    Q_DISABLE_COPY( MarbleModel )
    Q_PRIVATE_SLOT( d, void notifyModelChanged() )

    void addDownloadPolicies( GeoSceneDocument *mapTheme );
    GeoSceneTexture * textureLayer() const;

    MarbleModelPrivate  * const d;
};

}

#endif
