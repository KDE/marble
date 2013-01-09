//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2008 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#ifndef MARBLE_MARBLEWIDGET_H
#define MARBLE_MARBLEWIDGET_H


/** @file
 * This file contains the headers for MarbleWidget.
 *
 * @author Torsten Rahn <tackat@kde.org>
 * @author Inge Wallin  <inge@lysator.liu.se>
 */

#include <QtGui/QPixmap>
#include <QtGui/QWidget>

#include "GeoDataCoordinates.h"
#include "GeoDataLookAt.h"
#include "MarbleGlobal.h"             // types needed in all of marble.
#include "marble_export.h"
#include "GeoDataFolder.h"
// Qt
class QAbstractItemModel;
class QItemSelectionModel;
class QSettings;

namespace Marble
{

class AbstractDataPluginItem;
class AbstractFloatItem;
class GeoDataLatLonAltBox;
class GeoDataLatLonBox;
class GeoPainter;
class GeoSceneDocument;
class LayerInterface;
class MarbleModel;
class MarbleWidgetPopupMenu;
class MarbleWidgetInputHandler;
class MarbleWidgetPrivate;
class Quaternion;
class RenderPlugin;
class RoutingLayer;
class TextureLayer;
class TileCoordsPyramid;
class TileCreator;
class GeoDataPlacemark;
class ViewportParams;
class MapInfoDialog;

/**
 * @short A widget class that displays a view of the earth.
 *
 * This widget displays a view of the earth or any other globe,
 * depending on which dataset is used.  The user can navigate the
 * globe using either a control widget, e.g. the MarbleControlBox, or
 * the mouse.  The mouse and keyboard control is done through a
 * MarbleWidgetInputHandler. Only some aspects of the widget can be
 * controlled by the mouse and/or keyboard.
 *
 * By clicking on the globe and moving the mouse, the position can be
 * changed.  The user can also zoom by using the scroll wheel of the
 * mouse in the widget. The zoom value is not tied to any units, but
 * is an abstract value without any physical meaning. A value around
 * 1000 shows the full globe in a normal-sized window. Higher zoom
 * values give a more zoomed-in view.
 *
 * The MarbleWidget owns a data model to work. This model is contained
 * in the MarbleModel class, and it is painted by using a MarbleMap.
 * The widget takes care of creating the map and model. A MarbleModel
 * contains several datatypes, among them <b>tiles</b> which provide the
 * background, <b>vectors</b> which provide things like country
 * borders and coastlines and <b>placemarks</b> which can show points
 * of interest, such as cities, mountain tops or the poles.
 *
 * In addition to navigating with the mouse, you can also use it to
 * get information about items on the map. You can either click on a
 * placemark with the left mouse button or with the right mouse button
 * anywhere on the map.
 *
 * The left mouse button opens up a menu with all the placemarks
 * within a certain distance from the mouse pointer. When you choose
 * one item from the menu, Marble will open up a dialog window with
 * some information about the placemark and also try to connect to
 * Wikipedia to retrieve an article about it. If there is such an
 * article, you will get a mini-browser window with the article in a tab.
 *
 * @see MarbleControlBox
 * @see MarbleMap
 * @see MarbleModel
 */

class MARBLE_EXPORT MarbleWidget : public QWidget
{
    Q_OBJECT
#ifdef MARBLE_DBUS
    Q_CLASSINFO("D-Bus Interface", "org.kde.MarbleWidget")
#endif

    Q_PROPERTY(int zoom          READ zoom            WRITE setZoom)

    Q_PROPERTY(QString mapThemeId  READ mapThemeId    WRITE setMapThemeId)
    Q_PROPERTY(int projection    READ projection      WRITE setProjection)

    Q_PROPERTY(qreal longitude  READ centerLongitude WRITE setCenterLongitude)
    Q_PROPERTY(qreal latitude   READ centerLatitude  WRITE setCenterLatitude)

    Q_PROPERTY(bool showOverviewMap READ showOverviewMap    WRITE setShowOverviewMap)
    Q_PROPERTY(bool showScaleBar READ showScaleBar    WRITE setShowScaleBar)
    Q_PROPERTY(bool showCompass  READ showCompass     WRITE setShowCompass)
    Q_PROPERTY(bool showGrid     READ showGrid        WRITE setShowGrid)

    Q_PROPERTY(bool showClouds   READ showClouds      WRITE setShowClouds)
    Q_PROPERTY(bool showSunShading READ showSunShading WRITE setShowSunShading)
    Q_PROPERTY(bool showCityLights READ showCityLights WRITE setShowCityLights)
    Q_PROPERTY(bool isLockedToSubSolarPoint READ isLockedToSubSolarPoint WRITE setLockToSubSolarPoint)
    Q_PROPERTY(bool isSubSolarPointIconVisible READ isSubSolarPointIconVisible WRITE setSubSolarPointIconVisible)
    Q_PROPERTY(bool showAtmosphere READ showAtmosphere WRITE setShowAtmosphere)
    Q_PROPERTY(bool showCrosshairs READ showCrosshairs WRITE setShowCrosshairs)

    Q_PROPERTY(bool showPlaces   READ showPlaces      WRITE setShowPlaces)
    Q_PROPERTY(bool showCities   READ showCities      WRITE setShowCities)
    Q_PROPERTY(bool showTerrain  READ showTerrain     WRITE setShowTerrain)
    Q_PROPERTY(bool showOtherPlaces READ showOtherPlaces WRITE setShowOtherPlaces)

    Q_PROPERTY(bool showRelief   READ showRelief      WRITE setShowRelief)

    Q_PROPERTY(bool showIceLayer READ showIceLayer    WRITE setShowIceLayer)
    Q_PROPERTY(bool showBorders  READ showBorders     WRITE setShowBorders)
    Q_PROPERTY(bool showRivers   READ showRivers      WRITE setShowRivers)
    Q_PROPERTY(bool showLakes    READ showLakes       WRITE setShowLakes)

    Q_PROPERTY(quint64 volatileTileCacheLimit    READ volatileTileCacheLimit    WRITE setVolatileTileCacheLimit)

 public:

    /**
     * @brief Construct a new MarbleWidget.
     * @param parent the parent widget
     *
     * This constructor should be used when you will only use one
     * MarbleWidget.  The widget will create its own MarbleModel when
     * created.
     */
    explicit MarbleWidget( QWidget *parent = 0 );

    virtual ~MarbleWidget();

    /// @name Access to helper objects
    //@{

    /**
     * @brief Return the model that this view shows.
     */
    MarbleModel *model() const;

    ViewportParams *viewport();
    const ViewportParams *viewport() const;

    MarbleWidgetPopupMenu *popupMenu();

    /**
     * Returns the current input handler
     */
    MarbleWidgetInputHandler *inputHandler() const;

    /**
     * @brief Set the input handler
     */
    void setInputHandler( MarbleWidgetInputHandler *handler );

    /**
     * @brief Returns a list of all RenderPlugins on the widget, this includes float items
     * @return the list of RenderPlugins
     */
    QList<RenderPlugin *> renderPlugins() const;

    /**
     * @brief Returns a list of all FloatItems on the widget
     * @return the list of the floatItems
     */
    QList<AbstractFloatItem *> floatItems() const;

    /**
     * @brief Returns the FloatItem with the given id
     * @return The pointer to the requested floatItem,
     *
     * If no item is found the null pointer is returned.
     */
    AbstractFloatItem * floatItem( const QString &nameId ) const;

    /**
     * Reads the plugin settings from the passed QSettings.
     * You shouldn't use this in a KDE application as these use KConfig. Here you could
     * use MarblePart which is handling this automatically.
     * @param settings The QSettings object to be used.
     */
    void readPluginSettings( QSettings& settings );

    /**
     * Writes the plugin settings in the passed QSettings.
     * You shouldn't use this in a KDE application as these use KConfig. Here you could
     * use MarblePart which is handling this automatically.
     * @param settings The QSettings object to be used.
     */
    void writePluginSettings( QSettings& settings ) const;

    /**
     * @brief Retrieve the view context (i.e. still or animated map)
     */
    ViewContext viewContext() const;

    /**
     * @brief Get the GeoSceneDocument object of the current map theme
     */
    GeoSceneDocument * mapTheme() const;

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

    RoutingLayer* routingLayer();

    MapInfoDialog* mapInfoDialog();

    /**
     * @brief  Get the Projection used for the map
     * @return @c Spherical         a Globe
     * @return @c Equirectangular   a flat map
     * @return @c Mercator          another flat map
     */
    Projection projection() const;
//    int projection() const;

    //@}

    /// @name Visible map area
    //@{

    /**
     * @brief Get the ID of the current map theme
     * To ensure that a unique identifier is being used the theme does NOT
     * get represented by its name but the by relative location of the file
     * that specifies the theme:
     *
     * Example:
     *    mapThemeId = "earth/bluemarble/bluemarble.dgml"
     */
    QString mapThemeId() const;

    /**
     * @brief Return the projected region which describes the (shape of the) projected surface.
     */
    QRegion mapRegion() const;

    /**
     * @brief  Return the radius of the globe in pixels.
     */
    int radius() const;

    /**
     * @brief Return the current zoom amount.
     */
    int zoom() const;

    int tileZoomLevel() const;

    /**
     * @brief Return the current distance.
     */
    qreal distance() const;

    /**
     * @brief Return the current distance string.
     */
    QString distanceString() const;

    /**
     * @brief Return the minimum zoom value for the current map theme.
     */
    int minimumZoom() const;

    /**
     * @brief Return the minimum zoom value for the current map theme.
     */
    int maximumZoom() const;

    //@}

    /// @name Position management
    //@{

    /**
     * @brief Get the screen coordinates corresponding to geographical coordinates in the widget.
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
     * @brief Get the earth coordinates corresponding to a pixel in the widget.
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

    /**
     * @brief  Return how much the map will move if one of the move slots are called.
     * @return The move step.
     */
    qreal moveStep() const;

    /**
    * @brief Return the lookAt
    */
    GeoDataLookAt lookAt() const;

    /**
     * @return The current point of focus, e.g. the point that is not moved
     * when changing the zoom level. If not set, it defaults to the
     * center point.
     * @see centerLongitude centerLatitude setFocusPoint resetFocusPoint
     */
    GeoDataCoordinates focusPoint() const;

    /**
     * @brief Change the point of focus, overridding any previously set focus point.
     * @param focusPoint New focus point
     * @see focusPoint resetFocusPoint
     */
    void setFocusPoint( const GeoDataCoordinates &focusPoint );

    /**
     * @brief Invalidate any focus point set with @ref setFocusPoint.
     * @see focusPoint setFocusPoint
     */
    void resetFocusPoint();

    /**
      * @brief Return the globe radius (pixel) for the given distance (km)
      */
    qreal radiusFromDistance( qreal distance ) const;

    /**
      * @brief Return the distance (km) at the given globe radius (pixel)
      */
    qreal distanceFromRadius( qreal radius ) const;

    /**
      * Returns the zoom value (no unit) corresponding to the given camera distance (km)
      */
    qreal zoomFromDistance( qreal distance ) const;

    /**
      * Returns the distance (km) corresponding to the given zoom value
      */
    qreal distanceFromZoom( qreal zoom ) const;

    //@}

    /// @name Placemark management
    //@{

    QVector<const GeoDataPlacemark*> whichFeatureAt( const QPoint& ) const;

    //@}

    /// @name Float items and map appearance
    //@{

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
     * @brief  Return whether the globe is locked to the sub solar point
     * @return if globe is locked to sub solar point
     */
    bool isLockedToSubSolarPoint() const;

    /**
     * @brief  Return whether the sun icon is shown in the sub solar point.
     * @return visibility of the sun icon in the sub solar point
     */
    bool isSubSolarPointIconVisible() const;

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
     * @brief Retrieve the map quality depending on the view context
     */
    MapQuality mapQuality( ViewContext = Still ) const;

    /**
     * @brief Retrieve whether travels to a point should get animated
     */
    bool animationsEnabled() const;

    AngleUnit defaultAngleUnit() const;
    void setDefaultAngleUnit( AngleUnit angleUnit );

    QFont defaultFont() const;
    void setDefaultFont( const QFont& font );

    //@}

    /// @name Tile management
    //@{

    /**
     * @brief  Returns the limit in kilobytes of the volatile (in RAM) tile cache.
     * @return the limit of volatile tile cache
     */
    quint64 volatileTileCacheLimit() const;

    //@}

    /// @name Miscellaneous
    //@{

    /**
     * @brief  Return a QPixmap with the current contents of the widget.
     */
    QPixmap mapScreenShot();

    //@}

    /// @todo Enable this instead of the zoomView slot below for proper deprecation warnings
    /// around Marble 1.8
    // @deprecated Please use setZoom
    //MARBLE_DEPRECATED( void zoomView( int zoom, FlyToMode mode = Instant ) );

 public Q_SLOTS:

    /// @name Position management slots
    //@{

    /**
     * @brief  Set the radius of the globe in pixels.
     * @param  radius  The new globe radius value in pixels.
     */
    void setRadius( int radius );

    /**
     * @brief  Zoom the view to a certain zoomlevel
     * @param  zoom  the new zoom level.
     *
     * The zoom level is an abstract value without physical
     * interpretation.  A zoom value around 1000 lets the viewer see
     * all of the earth in the default window.
     */
    void setZoom( int zoom, FlyToMode mode = Instant );

    /**
     * @deprecated To be removed soon. Please use setZoom instead. Same parameters.
     */
    void zoomView( int zoom, FlyToMode mode = Instant );

    /**
     * @brief  Zoom the view by a certain step
     * @param  zoomStep  the difference between the old zoom and the new
     */
    void zoomViewBy( int zoomStep, FlyToMode mode = Instant );

        /**
     * @brief  Zoom in by the amount zoomStep.
     */
    void zoomIn( FlyToMode mode = Automatic );
    /**
     * @brief  Zoom out by the amount zoomStep.
     */
    void zoomOut( FlyToMode mode = Automatic );

    /**
     * @brief  Set the distance of the observer to the globe in km.
     * @param  distance  The new distance in km.
     */
    void setDistance( qreal distance );

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
    void rotateBy( const qreal deltaLon, const qreal deltaLat, FlyToMode mode = Instant );

    /**
     * @brief  Center the view on a geographical point
     * @param  lat  an angle in degrees parallel to the latitude lines
     *              +90(N) - -90(S)
     * @param  lon  an angle in degrees parallel to the longitude lines
     *              +180(W) - -180(E)
     */
    void centerOn( const qreal lon, const qreal lat, bool animated = false );

    /**
     * @brief  Center the view on a point
     * This method centers the Marble map on the point described by the latitude
     * and longitude in the GeoDataCoordinate parameter @c point. It also zooms
     * the map to be at the elevation described by the altitude. If this is
     * not the desired functionality or you do not have an accurate altitude
     * then use @see centerOn(qreal, qreal, bool)
     * @param point the point in 3 dimensions above the globe to move the view
     *              to. It will always be looking vertically down.
     */
    void centerOn( const GeoDataCoordinates &point, bool animated = false );

    /**
     * @brief Center the view on a bounding box so that it completely fills the viewport
     * This method not only centers on the center of the GeoDataLatLon box but it also
     * adjusts the zoom of the marble widget so that the LatLon box provided fills
     * the viewport.
     * @param box The GeoDataLatLonBox to zoom and move the MarbleWidget to.
     */
    void centerOn( const GeoDataLatLonBox& box, bool animated = false );

    /**
     * @brief Center the view on a placemark according to the following logic:
     * - if the placemark has a lookAt, zoom and center on that lookAt
     * - otherwise use the placemark geometry's latLonAltBox
     * @param box The GeoDataPlacemark to zoom and move the MarbleWidget to.
     */
    void centerOn( const GeoDataPlacemark& placemark, bool animated = false );

    /**
     * @brief  Set the latitude for the center point
     * @param  lat  the new value for the latitude in degree.
     * @param  mode the FlyToMode that will be used.
     */
    void setCenterLatitude( qreal lat, FlyToMode mode = Instant );

    /**
     * @brief  Set the longitude for the center point
     * @param  lon  the new value for the longitude in degree.
     * @param  mode the FlyToMode that will be used.
     */
    void setCenterLongitude( qreal lon, FlyToMode mode = Instant );

    /**
     * @brief  Move left by the moveStep.
     */
    void moveLeft( FlyToMode mode = Automatic );

    /**
     * @brief  Move right by the moveStep.
     */
    void moveRight( FlyToMode mode = Automatic );

    /**
     * @brief  Move up by the moveStep.
     */
    void moveUp( FlyToMode mode = Automatic );

    /**
     * @brief  Move down by the moveStep.
     */
    void moveDown( FlyToMode mode = Automatic );

    /**
     * @brief Center the view on the default start point with the default zoom.
     */
    void goHome( FlyToMode mode = Automatic );

    /**
      * @brief Change the camera position to the given position.
      * @param lookAt New camera position. Changing the camera position means
      * that both the current center position as well as the zoom value may change
      * @param mode Interpolation type for intermediate camera positions. Automatic
      * (default) chooses a suitable interpolation among Instant, Lenar and Jump.
      * Instant will directly set the new zoom and position values, while
      * Linear results in a linear interpolation of intermediate center coordinates
      * along the sphere and a linear interpolation of changes in the camera distance
      * to the ground. Finally, Jump will behave the same as Linear with regard to
      * the center position interpolation, but use a parabolic height increase
      * towards the middle point of the intermediate positions. This appears
      * like a jump of the camera.
      */
    void flyTo( const GeoDataLookAt &lookAt, FlyToMode mode = Automatic );

    //@}

    /// @name Float items and map appearance slots
    //@{

    /**
     * @brief  Set the Projection used for the map
     * @param  projection projection type (e.g. Spherical, Equirectangular, Mercator)
     */
    void setProjection( int        projection );
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
     * @brief  Set the globe locked to the sub solar point
     * @param  vsible if globe is locked to the sub solar point
     */
    void setLockToSubSolarPoint( bool visible );

    /**
     * @brief  Set whether the sun icon is shown in the sub solar point
     * @param  visible if the sun icon is shown in the sub solar point
     */
    void setSubSolarPointIconVisible( bool visible );

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
     * @brief Set whether the is tile is visible
     * NOTE: This is part of the transitional debug API
     *       and might be subject to changes until Marble 0.8
     * @param visible visibility of the tile
     */
    void setShowTileId( bool visible );

    /**
     * @brief Set whether the runtime tracing for layers gets shown
     * @param visible visibility of the runtime tracing
     */
    void setShowRuntimeTrace( bool visible );

    /**
     * @brief Set the map quality for the specified view context.
     *
     * @param quality map quality for the specified view context
     * @param viewContext view context whose map quality should be set
     */
    void setMapQualityForViewContext( MapQuality quality, ViewContext viewContext );

    /**
     * @brief Set the view context (i.e. still or animated map)
     */
    void setViewContext( ViewContext viewContext );

    /**
     * @brief Set whether travels to a point should get animated
     */
    void setAnimationsEnabled( bool enabled );

    //@}

    /// @name Tile management slots
    //@{

    void clearVolatileTileCache();
    /**
     * @brief  Set the limit of the volatile (in RAM) tile cache.
     * @param  kilobytes The limit in kilobytes.
     */
    void setVolatileTileCacheLimit( quint64 kiloBytes );

    /**
     * @brief A slot that is called when the model starts to create new tiles.
     * @param creator the tile creator object.
     * @param name  the name of the created theme.
     * @param description  a descriptive text that can be shown in a dialog.
     * @see    creatingTilesProgress
     *
     * This function is connected to the models signal with the same
     * name.  When the model needs to create a cache of tiles in
     * several different resolutions, it will emit creatingTilesStart
     * once with a name of the theme and a descriptive text.  The
     * widget can then pop up a dialog to explain why there is a
     * delay.  The model will then call creatingTilesProgress several
     * times until the parameter reaches 100 (100%), after which the
     * creation process is finished.  After this there will be no more
     * calls to creatingTilesProgress, and the poup dialog can then be
     * closed.
     */
    void creatingTilesStart( TileCreator *creator, const QString& name, const QString& description );

    /**
     * @brief Re-download all visible tiles.
     */
    void reloadMap();

    void downloadRegion( QVector<TileCoordsPyramid> const & );

    //@}

    /// @name Miscellaneous slots
    //@{

    /**
     * @brief Used to notify about the position of the mouse click
      */
    void notifyMouseClick( int x, int y );

    void setSelection( const QRect& region );

    void setInputEnabled( bool );

    const TextureLayer *textureLayer() const;

    //@}

 Q_SIGNALS:
    /**
     * @brief Signal that the zoom has changed, and to what.
     * @param zoom  The new zoom value.
     * @see  setZoom()
     */
    void zoomChanged( int zoom );
    void distanceChanged( const QString& distanceString );

    void tileLevelChanged( int level );

    /**
     * @brief Signal that the theme has changed
     * @param theme  Name of the new theme.
     */
    void themeChanged( const QString& theme );

    void projectionChanged( Projection );

    void mouseMoveGeoPosition( const QString& );

    void mouseClickGeoPosition( qreal lon, qreal lat, GeoDataCoordinates::Unit );

    void framesPerSecond( qreal fps );

    /** This signal is emit when a new rectangle region is selected over the map 
     *  The list of double values include coordinates in degrees using this order:
     *  lon1, lat1, lon2, lat2 (or West, North, East, South) as left/top, right/bottom rectangle.
     */
    void regionSelected( const QList<double>& );

    /**
     * This signal is emit when the settings of a plugin changed.
     */
    void pluginSettingsChanged();
    
    /**
     * @brief Signal that a render item has been initialized
     */
    void renderPluginInitialized( RenderPlugin *renderPlugin );

    /**
     * This signal is emitted when the visible region of the map changes. This typically happens
     * when the user moves the map around or zooms.
     */
    void visibleLatLonAltBoxChanged( const GeoDataLatLonAltBox& visibleLatLonAltBox );

 protected:
    /**
     * @brief Reimplementation of the leaveEvent() function in QWidget.
     */
    virtual void leaveEvent( QEvent *event );

    /**
     * @brief Reimplementation of the paintEvent() function in QWidget.
     */
    virtual void paintEvent( QPaintEvent *event );

    /**
     * @brief Reimplementation of the resizeEvent() function in QWidget.
     */
    virtual void resizeEvent( QResizeEvent *event );

    void connectNotify( const char * signal );
    void disconnectNotify( const char * signal );

    /**
      * @brief Reimplementation of the changeEvent() function in QWidget to
      * react to changes of the enabled state
      */
    virtual void changeEvent( QEvent * event );

    /**
     * @brief Enables custom drawing onto the MarbleWidget straight after
     * @brief the globe and before all other layers has been rendered.
     * @param painter
     *
     * @deprecated implement LayerInterface and add it using @p addLayer()
     */
    virtual void customPaint( GeoPainter *painter );

 private:
    Q_PRIVATE_SLOT( d, void updateMapTheme() )
    Q_PRIVATE_SLOT( d, void updateSystemBackgroundAttribute() )

 private:
    Q_DISABLE_COPY( MarbleWidget )
    MarbleWidgetPrivate  * const d;
    friend class MarbleWidgetPrivate;

    class CustomPaintLayer;
    friend class CustomPaintLayer;

    friend class MarbleWidgetDefaultInputHandler;
};

}

#endif
