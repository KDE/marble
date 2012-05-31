//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2008 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2009      Jens-Michael Hoffmann <jensmh@gmx.de>
//

#ifndef MARBLE_MARBLEMAP_H
#define MARBLE_MARBLEMAP_H


/** @file
 * This file contains the headers for MarbleMap.
 *
 * @author Torsten Rahn <tackat@kde.org>
 * @author Inge Wallin  <inge@lysator.liu.se>
 */


#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtGui/QFont>
#include <QtGui/QPixmap>

#include "marble_export.h"
#include "GeoDataCoordinates.h"       // In geodata/data/

// Qt
class QAbstractItemModel;
class QItemSelectionModel;

namespace Marble
{

// MarbleMap 
class MarbleMapPrivate;

// Marble
class GeoDataLatLonAltBox;
class GeoDataPlacemark;
class MarbleModel;
class ViewportParams;
class GeoPainter;
class LayerInterface;
class Quaternion;
class RenderPlugin;
class AbstractDataPlugin;
class AbstractDataPluginItem;
class AbstractFloatItem;
class TextureLayer;
class TileCoordsPyramid;

/**
 * @short A class that can paint a view of the earth.
 *
 * FIXME: Change this description when we are done.
 *
 * This class can paint a view of the earth or any other globe,
 * depending on which dataset is used. It can be used to show the
 * globe in a widget like MarbleWidget does, or on any other
 * QPaintDevice.
 *
 * The projection and other view parameters that control how MarbleMap
 * paints the map is given through the class ViewParams. If the
 * programmer wants to allow the user to control the map, he/she has
 * to provide a way for the user to interact with it.  An example of
 * this can be seen in the class MarbleWidgetInputHandler, that lets
 * the user control a MarbleWidget that uses MarbleMap internally.
 *
 * The MarbleMap needs to be provided with a data model to
 * work. This model is contained in the MarbleModel class. The widget
 * can also construct its own model if none is given to the
 * constructor.  This data model contains 3 separate datatypes:
 * <b>tiles</b> which provide the background, <b>vectors</b> which
 * provide things like country borders and coastlines and
 * <b>placemarks</b> which can show points of interest, such as
 * cities, mountain tops or the poles.
 *
 * @see MarbleWidget
 * @see MarbleControlBox
 * @see MarbleModel
 */

class MARBLE_EXPORT MarbleMap : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.MarbleMap")

 public:

    friend class MarbleWidget;

    /**
     * @brief Construct a new MarbleMap.
     *
     * This constructor should be used when you will only use one
     * MarbleMap.  The widget will create its own MarbleModel when
     * created.
     */
    MarbleMap();

    /**
     * @brief Construct a new MarbleMap.
     * @param model  the data model for the widget.
     *
     * This constructor should be used when you plan to use more than
     * one MarbleMap for the same MarbleModel (not yet supported,
     * but will be soon).
     */
    explicit MarbleMap( MarbleModel *model );

    virtual ~MarbleMap();

    /**
     * @brief Return the model that this view shows.
     */
    MarbleModel *model() const;

    // Accessors to internal objects;
    ViewportParams *viewport();
    const ViewportParams *viewport() const;

    /**
     * @brief  Get the Projection used for the map
     * @return @c Spherical         a Globe
     * @return @c Equirectangular   a flat map
     * @return @c Mercator          another flat map
     */
    Projection projection() const;

    /**
     * @brief Get the ID of the current map theme
     * To ensure that a unique identifier is being used the theme does NOT
     * get represented by its name but the by relative location of the file
     * that specifies the theme:
     *
     * Example:
     *    maptheme = "earth/bluemarble/bluemarble.dgml"
     */
    QString mapThemeId() const;

    void setMapQualityForViewContext( MapQuality qualityForViewContext, ViewContext viewContext );
    MapQuality mapQuality( ViewContext viewContext ) const;

    /**
     * @brief Return the current map quality.
     */
    MapQuality mapQuality() const;

    void setViewContext( ViewContext viewContext );
    ViewContext viewContext() const;

    void setSize( int width, int height );
    void setSize( const QSize& size );
    QSize size() const;
    int width() const;
    int height() const;

    /**
     * @brief  Return the radius of the globe in pixels.
     */
    int radius() const;

    int preferredRadiusCeil( int radius );
    int preferredRadiusFloor( int radius );

    int tileZoomLevel() const;

    /**
     * @brief return the minimum zoom value for the current map theme.
     */
    int minimumZoom() const;

    /**
     * @brief return the minimum zoom value for the current map theme.
     */
    int maximumZoom() const;

    /**
     * @brief Get the screen coordinates corresponding to geographical coordinates in the map.
     * @param lon    the lon coordinate of the requested pixel position
     * @param lat    the lat coordinate of the requested pixel position
     * @param x      the x coordinate of the pixel is returned through this parameter
     * @param y      the y coordinate of the pixel is returned through this parameter
     * @return @c true  if the geographical coordinates are visible on the screen
     *         @c false if the geographical coordinates are not visible on the screen
     */
    bool screenCoordinates( qreal lon, qreal lat,
                            qreal& x, qreal& y ) const;

    /**
     * @brief Get the earth coordinates corresponding to a pixel in the map.
     * @param x      the x coordinate of the pixel
     * @param y      the y coordinate of the pixel
     * @param lon    the longitude angle is returned through this parameter
     * @param lat    the latitude angle is returned through this parameter
     * @return @c true  if the pixel (x, y) is within the globe
     *         @c false if the pixel (x, y) is outside the globe, i.e. in space.
     */
    bool geoCoordinates( int x, int y,
                         qreal& lon, qreal& lat,
                         GeoDataCoordinates::Unit = GeoDataCoordinates::Degree ) const;

    /**
     * @brief Return the longitude of the center point.
     * @return The longitude of the center point in degree.
     */
    qreal centerLongitude() const;

    /**
     * @brief Return the latitude of the center point.
     * @return The latitude of the center point in degree.
     */
    qreal centerLatitude() const;

    QVector<const GeoDataPlacemark*> whichFeatureAt( const QPoint& ) const;

    /**
     * @brief  Return the property value by name.
     * @return The property value (usually: visibility).
     */
    bool propertyValue( const QString& name ) const;

    /**
     * @brief  Return whether the overview map is visible.
     * @return The overview map visibility.
     */
    bool showOverviewMap() const;

    /**
     * @brief  Return whether the scale bar is visible.
     * @return The scale bar visibility.
     */
    bool showScaleBar() const;

    /**
     * @brief  Return whether the compass bar is visible.
     * @return The compass visibility.
     */
    bool showCompass() const;

    /**
     * @brief  Return whether the cloud cover is visible.
     * @return The cloud cover visibility.
     */
    bool showClouds() const;

    /**
     * @brief  Return whether the night shadow is visible.
     * @return visibility of night shadow
     */
    bool showSunShading() const;

    /**
     * @brief  Return whether the city lights are shown instead of the night shadow.
     * @return visibility of city lights
     */
    bool showCityLights() const;

    /**
     * @brief  Return whether the sun is shown in the zenith.
     * @return visibility of sun in the zenith
     */
    bool showSunInZenith() const;

    /**
     * @brief  Return whether the atmospheric glow is visible.
     * @return The cloud cover visibility.
     */
    bool showAtmosphere() const;

    /**
     * @brief  Return whether the crosshairs are visible.
     * @return The crosshairs' visibility.
     */
    bool showCrosshairs() const;

    /**
     * @brief  Return whether the coordinate grid is visible.
     * @return The coordinate grid visibility.
     */
    bool showGrid() const;

    /**
     * @brief  Return whether the place marks are visible.
     * @return The place mark visibility.
     */
    bool showPlaces() const;

    /**
     * @brief  Return whether the city place marks are visible.
     * @return The city place mark visibility.
     */
    bool showCities() const;

    /**
     * @brief  Return whether the terrain place marks are visible.
     * @return The terrain place mark visibility.
     */
    bool showTerrain() const;

    /**
     * @brief  Return whether other places are visible.
     * @return The visibility of other places.
     */
    bool showOtherPlaces() const;

    /**
     * @brief  Return whether the relief is visible.
     * @return The relief visibility.
     */
    bool showRelief() const;

    /**
     * @brief  Return whether the ice layer is visible.
     * @return The ice layer visibility.
     */
    bool showIceLayer() const;

    /**
     * @brief  Return whether the borders are visible.
     * @return The border visibility.
     */
    bool showBorders() const;

    /**
     * @brief  Return whether the rivers are visible.
     * @return The rivers' visibility.
     */
    bool showRivers() const;

    /**
     * @brief  Return whether the lakes are visible.
     * @return The lakes' visibility.
     */
    bool showLakes() const;

    /**
     * @brief  Return whether the frame rate gets displayed.
     * @return the frame rates visibility
     */
    bool showFrameRate() const;

    bool showBackground() const;

    /**
     * @brief  Returns the limit in kilobytes of the volatile (in RAM) tile cache.
     * @return the limit of volatile tile cache in kilobytes.
     */
    quint64 volatileTileCacheLimit() const;

    /**
     * @brief Returns a list of all RenderPlugins in the model, this includes float items
     * @return the list of RenderPlugins
     */
    QList<RenderPlugin *> renderPlugins() const;
    QList<AbstractFloatItem *> floatItems() const;

    /**
     * @brief Returns a list of all FloatItems in the model
     * @return the list of the floatItems
     */
    AbstractFloatItem * floatItem( const QString &nameId ) const;

    /**
     * @brief Returns a list of all DataPlugins on the layer
     * @return the list of DataPlugins
     */
    QList<AbstractDataPlugin *> dataPlugins()  const;

    /**
     * @brief Returns all widgets of dataPlugins on the position curpos
     */
    QList<AbstractDataPluginItem *> whichItemAt( const QPoint& curpos ) const;

    AngleUnit defaultAngleUnit() const;

    QFont defaultFont() const;

    const TextureLayer *textureLayer() const;

    /**
     * @brief Add a layer to be included in rendering.
     */
    void addLayer( LayerInterface *layer );

    /**
     * @brief Remove a layer from being included in rendering.
     */
    void removeLayer( LayerInterface *layer );

 public Q_SLOTS:

    /**
     * @brief Paint the map using a give painter.
     * @param painter  The painter to use.
     * @param dirtyRect the rectangle that actually needs repainting.
     */
    void paint( GeoPainter &painter, const QRect &dirtyRect );

    /**
     * @brief  Set the radius of the globe in pixels.
     * @param  radius  The new globe radius value in pixels.
     */
    void setRadius( int radius );

    /**
     * @brief  Rotate the view by the two angles phi and theta.
     * @param  deltaLon  an angle that specifies the change in terms of longitude
     * @param  deltaLat  an angle that specifies the change in terms of latitude
     *
     * This function rotates the view by two angles,
     * deltaLon ("theta") and deltaLat ("phi").
     * If we start at (0, 0), the result will be the exact equivalent
     * of (lon, lat), otherwise the resulting angle will be the sum of
     * the previous position and the two offsets.
     */
    void rotateBy( const qreal &deltaLon, const qreal &deltaLat );

    /**
     * @brief  Center the view on a geographical point
     * @param  lat  an angle parallel to the latitude lines
     *              +90(N) - -90(S)
     * @param  lon  an angle parallel to the longitude lines
     *              +180(W) - -180(E)
     */
    void centerOn( const qreal lon, const qreal lat );

    /**
     * @brief  Set the latitude for the center point
     * @param  lat  the new value for the latitude in degree
     */
    void setCenterLatitude( qreal lat );

    /**
     * @brief  Set the longitude for the center point
     * @param  lon  the new value for the longitude in degree
     */
    void setCenterLongitude( qreal lon );

    /**
     * @brief  Set the Projection used for the map
     * @param  projection projection type (e.g. Spherical, Equirectangular, Mercator)
     */
    void setProjection( Projection projection );

    /**
     * @brief Set a new map theme
     * @param maptheme  The ID of the new maptheme. To ensure that a unique 
     * identifier is being used the theme does NOT get represented by its 
     * name but the by relative location of the file that specifies the theme:
     *
     * Example: 
     *    maptheme = "earth/bluemarble/bluemarble.dgml" 
     */
    void setMapThemeId( const QString& maptheme );

    /**
     * @brief  Sets the value of a map theme property
     * @param  value  value of the property (usually: visibility)
     * 
     * Later on we might add a "setPropertyType and a QVariant
     * if needed.
     */
    void setPropertyValue( const QString& name, bool value );

    /**
     * @brief  Set whether the overview map overlay is visible
     * @param  visible  visibility of the overview map
     */
    void setShowOverviewMap( bool visible );

    /**
     * @brief  Set whether the scale bar overlay is visible
     * @param  visible  visibility of the scale bar
     */
    void setShowScaleBar( bool visible );

    /**
     * @brief  Set whether the compass overlay is visible
     * @param  visible  visibility of the compass
     */
    void setShowCompass( bool visible );

    /**
     * @brief  Set whether the cloud cover is visible
     * @param  visible  visibility of the cloud cover
     */
    void setShowClouds( bool visible );

    /**
     * @brief  Set whether the night shadow is visible.
     * @param  visibile visibility of shadow
     */
    void setShowSunShading( bool visible );

    /**
     * @brief  Set whether city lights instead of night shadow are visible.
     * @param  visible visibility of city lights
     */
    void setShowCityLights( bool visible );

    /**
     * @brief Set whether the sun is visible in the zenith.
     * @param visible  visibility of the sun in the zenith
     */
    void setShowSunInZenith( bool visible );

    /**
     * @brief Set whether the is tile is visible
     * NOTE: This is part of the transitional debug API
     *       and might be subject to changes until Marble 0.8
     * @param visible visibility of the tile
     */ 
    void setShowTileId( bool visible );

    /**
     * @brief  Set whether the atmospheric glow is visible
     * @param  visible  visibility of the atmospheric glow
     */
    void setShowAtmosphere( bool visible );

    /**
     * @brief  Set whether the crosshairs are visible
     * @param  visible  visibility of the crosshairs
     */
    void setShowCrosshairs( bool visible );

    /**
     * @brief  Set whether the coordinate grid overlay is visible
     * @param  visible  visibility of the coordinate grid
     */
    void setShowGrid( bool visible );

    /**
     * @brief  Set whether the place mark overlay is visible
     * @param  visible  visibility of the place marks
     */
    void setShowPlaces( bool visible );

    /**
     * @brief  Set whether the city place mark overlay is visible
     * @param  visible  visibility of the city place marks
     */
    void setShowCities( bool visible );

    /**
     * @brief  Set whether the terrain place mark overlay is visible
     * @param  visible  visibility of the terrain place marks
     */
    void setShowTerrain( bool visible );

    /**
     * @brief  Set whether the other places overlay is visible
     * @param  visible  visibility of other places
     */
    void setShowOtherPlaces( bool visible );

    /**
     * @brief  Set whether the relief is visible
     * @param  visible  visibility of the relief
     */
    void setShowRelief( bool visible );

    /**
     * @brief  Set whether the ice layer is visible
     * @param  visible  visibility of the ice layer
     */
    void setShowIceLayer( bool visible );

    /**
     * @brief  Set whether the borders visible
     * @param  visible  visibility of the borders
     */
    void setShowBorders( bool visible );

    /**
     * @brief  Set whether the rivers are visible
     * @param  visible  visibility of the rivers
     */
    void setShowRivers( bool visible );

    /**
     * @brief  Set whether the lakes are visible
     * @param  visible  visibility of the lakes
     */
    void setShowLakes( bool visible );

    /**
     * @brief Set whether the frame rate gets shown
     * @param visible  visibility of the frame rate
     */
    void setShowFrameRate( bool visible );

    void setShowBackground( bool visible );

     /**
     * @brief used to notify about the position of the mouse click
      */
    void notifyMouseClick( int x, int y );

    void clearVolatileTileCache();
    /**
     * @brief  Set the limit of the volatile (in RAM) tile cache.
     * @param  bytes The limit in kilobytes.
     */
    void setVolatileTileCacheLimit( quint64 kiloBytes );

    void setDefaultAngleUnit( AngleUnit angleUnit );

    void setDefaultFont( const QFont& font );

    /**
     * @brief Reload the currently displayed map by reloading texture tiles
     *        from the Internet. In the future this should be extended to all
     *        kinds of data which is used in the map.
     */
    void reload() const;

    void downloadRegion( QString const & sourceDir, QVector<TileCoordsPyramid> const & );

 Q_SIGNALS:
    void tileLevelChanged( int level );

    /**
     * @brief Signal that the theme has changed
     * @param theme  Name of the new theme.
     */
    void themeChanged( const QString& theme );

    void projectionChanged( Projection );

    void radiusChanged( int radius );

    void mouseMoveGeoPosition( const QString& );

    void mouseClickGeoPosition( qreal lon, qreal lat, GeoDataCoordinates::Unit );

    void framesPerSecond( qreal fps );

    /**
     * This signal is emitted when the repaint of the view was requested.
     * If available with the @p dirtyRegion which is the region the view will change in.
     * If dirtyRegion.isEmpty() returns true, the whole viewport has to be repainted.
     */
    void repaintNeeded( const QRegion& dirtyRegion = QRegion() );

    /**
     * This signal is emitted when the visible region of the map changes. This typically happens
     * when the user moves the map around or zooms.
     */
    void visibleLatLonAltBoxChanged( const GeoDataLatLonAltBox& visibleLatLonAltBox );

    /**
     * @brief This signal is emit when the settings of a plugin changed.
     */
    void pluginSettingsChanged();

    /**
     * @brief Signal that a render item has been initialized
     */
    void renderPluginInitialized( RenderPlugin *renderPlugin );

 protected:

    /**
     * @brief Enables custom drawing onto the MarbleMap straight after
     * @brief the globe and before all other layers have been rendered.
     * @param painter
     *
     * @deprecated implement LayerInterface and add it using @p addLayer()
     */
    virtual void customPaint( GeoPainter *painter );

 private:
    Q_PRIVATE_SLOT( d, void updateMapTheme() )
    Q_PRIVATE_SLOT( d, void updateProperty( const QString &, bool ) )

 private:
    Q_DISABLE_COPY( MarbleMap )
    MarbleMapPrivate * const d;
    friend class MarbleMapPrivate;

    class CustomPaintLayer;
    friend class CustomPaintLayer;
};

}

#endif
