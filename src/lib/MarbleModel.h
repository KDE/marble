//
// This file is part of the Marble Virtual Globe.
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
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtGui/QRegion>

#include "global.h"

class QItemSelectionModel;
class QAbstractItemModel;
class QDateTime;
class QPoint;
class QRect;
class QTextDocument;

namespace Marble
{

class AbstractDataPlugin;
class AbstractDataPluginItem;
class GeoPainter;
class MeasureTool;
class MapThemeManager;
class TileCoordsPyramid;
class FileViewModel;
class PositionTracking;
class HttpDownloadManager;
class MarbleModelPrivate;
class PlacemarkLayout;
class MarbleClock;
class SunLocator;
class TextureLayer;
class TileCreator;
class ViewParams;
class StackedTile;
class RenderPlugin;
class PluginManager;
class AbstractFloatItem;
class GeoDataCoordinates;
class GeoDataDocument;
class GeoDataTreeModel;
class GeoSceneDocument;
class GeoSceneTexture;
class Planet;
class LayerInterface;
class RoutingManager;
class BookmarkManager;

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

    Q_PROPERTY( bool workOffline READ workOffline WRITE setWorkOffline NOTIFY workOfflineChanged )

 public:
    /**
     * @brief  Construct a new MarbleModel.
     * @param parent the parent widget
     */
    explicit MarbleModel( QObject *parent );
    virtual ~MarbleModel();

    /**
     * @brief Return the list of Placemarks as a QAbstractItemModel *
     * @return a list of all Placemarks in the MarbleModel.
     */
    GeoDataTreeModel*  treeModel() const;
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
    void setMapTheme( GeoSceneDocument* mapTheme );

    /**
     * @brief  get the home point
     * @param  lon  the longitude of the home point.
     * @param  lat  the latitude of the home point.
     * @param  zoom the default zoom level of the home point.
     */
    void home( qreal &lon, qreal &lat, int& zoom );
    /**
     * @brief  Set the home point
     * @param  lon  the longitude of the new home point.
     * @param  lat  the latitude of the new home point.
     * @param  zoom the default zoom level for the new home point.
     */
    void setHome( qreal lon, qreal lat, int zoom = 1050 );
    /**
     * @brief  Set the home point
     * @param  homePoint  the new home point.
     * @param  zoom       the default zoom level for the new home point.
     */
    void setHome( const GeoDataCoordinates& homePoint, int zoom = 1050 );

    MapThemeManager* mapThemeManager() const;

    /**
     * @brief Return the downloadmanager to load missing tiles
     * @return the HttpDownloadManager instance.
     */
    HttpDownloadManager* downloadManager() const;

    /**
      * @deprecated Please use addGeoDataFile instead
      */
    MARBLE_DEPRECATED( void openGpxFile( const QString& filename ) );

    /**
      * @deprecated Please use addGeoDataFile instead
      */
    MARBLE_DEPRECATED( void addPlacemarkFile( const QString& filename ) );

    /**
      * @deprecated Please use addGeoDataString instead
      */
    MARBLE_DEPRECATED( void addPlacemarkData( const QString& data, const QString& key = "data" ) );

    /**
      * @deprecated Please use removeGeoData instead
      */
    MARBLE_DEPRECATED( void removePlacemarkKey( const QString& key ) );

    void addGeoDataFile( const QString& filename );
    void addGeoDataString( const QString& data, const QString& key = "data" );
    void removeGeoData( const QString& key );

    FileViewModel      *fileViewModel()   const;
    PositionTracking   *positionTracking() const;

    qreal                 planetRadius()   const;
    QString               planetName()     const;
    QString               planetId()       const;

    MarbleClock *clock();
    const MarbleClock *clock() const;
    SunLocator*           sunLocator()     const;

    /**
     * @brief  Returns the limit in kilobytes of the persistent (on hard disc) tile cache.
     * @return the limit of persistent tile cache in kilobytes.
     */
    quint64 persistentTileCacheLimit() const;

    /**
     * @brief  Returns the limit of the volatile (in RAM) tile cache.
     * @return the cache limit in kilobytes
     */
    quint64 volatileTileCacheLimit() const;

    PluginManager* pluginManager() const;

    /**
     * @brief Returns the planet object for the current map.
     * @return the planet object for the current map
     */
    const Planet *planet() const;

    RoutingManager* routingManager();

    void setClockDateTime( const QDateTime& datetime );

    QDateTime clockDateTime() const;

    int clockSpeed() const;

    void setClockSpeed( int speed );

    void setClockTimezone( int timeInSec );

    int clockTimezone() const;

    /**
     * return instance of BookmarkManager
    */
    BookmarkManager *bookmarkManager() const;

    QTextDocument * legend();

    void setLegend( QTextDocument * document );

    bool workOffline() const;

    void setWorkOffline( bool workOffline );

 public Q_SLOTS:
    void clearPersistentTileCache();

    /**
     * @brief  Set the limit of the persistent (on hard disc) tile cache.
     * @param  bytes The limit in kilobytes, 0 means no limit.
     */
    void setPersistentTileCacheLimit( quint64 kiloBytes );

 Q_SIGNALS:

    /**
     * @brief Signal that the MarbleModel has started to create a new set of tiles.
     * @param
     * @see  zoomView()
     */
    void creatingTilesStart( TileCreator*, const QString& name, const QString& description );

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

    void workOfflineChanged();

 private:
    Q_DISABLE_COPY( MarbleModel )
    Q_PRIVATE_SLOT( d, void notifyModelChanged() )

    void addDownloadPolicies( GeoSceneDocument *mapTheme );
    MarbleModelPrivate  * const d;
};

}

#endif
