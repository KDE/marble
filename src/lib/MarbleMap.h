//
// This file is part of the Marble Desktop Globe.
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
#include "GeoDataLookAt.h"
#include "Quaternion.h"
#include "global.h"             // types needed in all of marble.

// Qt
class QAbstractItemModel;
class QModelIndex;
class QItemSelectionModel;

namespace Marble
{

// MarbleMap 
//class MarbleWidgetInputHandler;
//class MarbleWidgetPopupMenu;
class MarbleMapPrivate;

// Marble
class GeoDataLatLonAltBox;
class MarbleModel;
class ViewParams;
class MeasureTool;
class SunLocator;
class GpxFileModel;
class FileViewModel;
class GeoPainter;
class RenderPlugin;
class AbstractFloatItem;

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
    ViewParams *viewParams();
    MeasureTool *measureTool();

    void setSize( int width, int height );
    void setSize( const QSize& size );
    QSize size() const;
    int width() const;
    int height() const;

    /**
     * @brief  Return the radius of the globe in pixels.
     */
    int radius() const;

    /**
     * @brief  Set the radius of the globe in pixels.
     * @param  radius  The new globe radius value in pixels.
     */
    void setRadius( int radius );

    /**
     * @brief Return the current zoom level.
     */
    int zoom() const;

    /**
     * @brief Return the current distance. Convenience function calling distance(radius())
     * @see distance(qreal) radius
     */
    qreal distance() const;

    /**
     * @brief  Set the distance of the observer to the globe in km.
     * @param  distance  The new distance in km.
     */
    void setDistance( qreal distance );

    /**
     * @brief Return the current distance string.
     */
    QString distanceString() const;

    /**
     * @brief return the minimum zoom value for the current map theme.
     */
    int minimumZoom() const;

    /**
     * @brief return the minimum zoom value for the current map theme.
     */
    int maximumZoom() const;

    /**
     * @brief return if the map needs to be updated.
     */
    bool needsUpdate() const;

    /**
     * @brief Mark the map as needing an update.
     */
    void setNeedsUpdate();

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
                            qreal& x, qreal& y );

    /**
     * @brief Get the Y coordinate of the North Pole.
     * @return the pixel address of the geographical north pole.
     */
    int northPoleY();

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
                         GeoDataCoordinates::Unit = GeoDataCoordinates::Degree );

    /**
     * @brief Return the longitude of the center point.
     */
    qreal centerLongitude() const;

    /**
     * @brief Return the latitude of the center point.
     */
    qreal centerLatitude() const;

    /**
     * @brief returns the model for all the placemarks on the globe.
     */
    QAbstractItemModel *placemarkModel() const;

    /**
     * @brief returns the selection model for all the placemarks on the globe.
     */
    QItemSelectionModel *placemarkSelectionModel() const;

    /**
     * @brief  Return how much the map will move if one of the move slots are called.
     * @return The move step.
     */
    qreal moveStep();

    /**
     * @brief  Add a GeoDataPlacemark file to the model.
     * @param  filename  the filename of the file containing the Placemarks.
     */
    void addPlacemarkFile( const QString &filename );

    /**
     * @brief  Add GeoDataPlacemark data as string to the model.
     * @param  data  the string containing the Placemarks.
     * @param key  the string needed to identify the data
     */
    void addPlacemarkData( const QString& data, const QString& key = "data" );
    
    /**
     * @brief  remove data or files from the model.
     * @param key  either the filename or the string used to identify the data in addPlacemarkFile and addPlacemarkData
     */
    void removePlacemarkKey( const QString& key );

    /**
     * @brief  Return the quaternion that specifies the rotation of the globe.
     * @return The quaternion that describes the rotation of the globe.
     */
    Quaternion planetAxis() const;

    /**
     * @brief  Return a QPixmap with the current contents of the map.
     */
    QPixmap mapScreenShot();

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
     * @brief  Return whether the elevation model is visible.
     * @return The elevation model visibility.
     */
    bool showElevationModel() const;

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
     * @brief Return whether Gps Data is visible.
     * @return The Gps Data's visibility.
     */
    bool showGps() const;

    /**
     * @brief  Return whether the frame rate gets displayed.
     * @return the frame rates visibility
     */
    bool showFrameRate() const;

    /**
     * @brief  Returns the limit in kilobytes of the persistent (on hard disc) tile cache.
     * @return the limit of persistent tile cache in kilobytes.
     */
    quint64 persistentTileCacheLimit() const;

    /**
     * @brief  Returns the limit in kilobytes of the volatile (in RAM) tile cache.
     * @return the limit of volatile tile cache in kilobytes.
     */
    quint64 volatileTileCacheLimit() const;

    /**
     * @brief  Return the sun locator object.
     * @return the sun locator object
     */
    SunLocator* sunLocator();

    QList<RenderPlugin *> renderPlugins() const;
    QList<AbstractFloatItem *> floatItems() const;

    /**
      * @brief Move camera to the given position. This can change
      * both the zoom value and the position
      */
    void flyTo( const GeoDataLookAt &lookAt );

    /**
      * @brief Return the current camera position
      */
    GeoDataLookAt lookAt() const;

    /**
      * @brief Return the distance (km) at the given globe radius (pixel)
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

 public Q_SLOTS:

    void updateSun();
    void centerSun();

    /**
     * @brief Paint the map using a give painter.
     * @param painter  The painter to use.
     * @param dirtyRect the rectangle that actually needs repainting.
     */
    void paint( GeoPainter &painter, QRect &dirtyRect );

    /**
     * @brief  Zoom the view to a certain zoomlevel
     * @param  zoom  the new zoom level.
     *
     * The zoom level is an abstract value without physical
     * interpretation.  A zoom value around 1000 lets the viewer see
     * all of the earth in the default window.
     */
    void zoomView( int zoom );

    /**
     * @brief  Zoom the view by a certain step
     * @param  zoomStep  the difference between the old zoom and the new
     */
    void zoomViewBy( int zoomStep );

    /**
     * @brief  Zoom in by the amount zoomStep.
     */
    void zoomIn();
    /**
     * @brief  Zoom out by the amount zoomStep.
     */
    void zoomOut();

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
     * @brief  Rotate the view by the angle specified by a Quaternion.
     * @param  incRot a quaternion specifying the rotation
     */
    void rotateBy( const Quaternion& incRot );

    /**
     * @brief  Center the view on a geographical point
     * @param  lat  an angle parallel to the latitude lines
     *              +90(N) - -90(S)
     * @param  lon  an angle parallel to the longitude lines
     *              +180(W) - -180(E)
     */
    void centerOn( const qreal lon, const qreal lat );

    /**
     * @brief  Center the view on a point
     * @param  index  an index for a QModel, indicating a city
     */
    void centerOn( const QModelIndex& index );

    /**
     * @brief  Set the latitude for the center point
     * @param  lat  the new value for the latitude
     */
    void setCenterLatitude( qreal lat );

    /**
     * @brief  Set the longitude for the center point
     * @param  lon  the new value for the longitude
     */
    void setCenterLongitude( qreal lon );

    /**
     * @brief  Get the Projection used for the map
     * @return @c Spherical         a Globe
     * @return @c Equirectangular   a flat map
     * @return @c Mercator          another flat map
     */
    Projection projection() const;

    /**
     * @brief  Set the Projection used for the map
     * @param  projection projection type (e.g. Spherical, Equirectangular, Mercator)
     */
    void setProjection( Projection projection );

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

    /**
     * @brief  Move left by the moveStep.
     */
    void moveLeft();
    /**
     * @brief  Move right by the moveStep.
     */
    void moveRight();
    /**
     * @brief  Move up by the moveStep.
     */
    void moveUp();
    /**
     * @brief  Move down by the moveStep.
     */
    void moveDown();

    /**
     * @brief Center the view on the default start point with the default zoom.
     */
    void goHome();

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
     * @brief  Set whether the elevation model is visible
     * @param  visible  visibility of the elevation model
     */
    void setShowElevationModel( bool visible );

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
     * @brief Set whether Gps Data is visible
     * @param visible  visibility of the Gps Data
     */
    void setShowGps( bool visible );

    /**
     * @brief Set whether the frame rate gets shown
     * @param visible  visibility of the frame rate
     */
    void setShowFrameRate( bool visible );

    /**
     * @brief Set the current Gps position
     * @param lat the new latitude value
     * @param lon the new longitude value
     */
    void changeCurrentPosition( qreal lon, qreal lat );

     /**
     * @brief used to notify about the position of the mouse click
      */
    void notifyMouseClick( int x, int y );

    /**
     * @brief updates the gps tracking point by polling
     */
    void updateGps();

    /**
     * @brief Opens a gpx file for viewing on the Marble Map
     */
    void openGpxFile( const QString &filename );

    /**
     * @brief Return a QAbstractItemModel containing GPX files.
     */
    GpxFileModel *gpxFileModel();

    /**
     * @brief Return a QAbstractItemModel containing files.
     */
    FileViewModel* fileViewModel() const;

    void clearPersistentTileCache();
    /**
     * @brief  Set the limit of the persistent (on hard disc) tile cache.
     * @param  bytes The limit in kilobytes, 0 means no limit.
     */
    void setPersistentTileCacheLimit( quint64 kiloBytes );

    void clearVolatileTileCache();
    /**
     * @brief  Set the limit of the volatile (in RAM) tile cache.
     * @param  bytes The limit in kilobytes.
     */
    void setVolatileTileCacheLimit( quint64 kiloBytes );

    /**
     * @brief Update the map because the model changed.
     */
    void updateChangedMap();

    bool mapCoversViewport();

    AngleUnit defaultAngleUnit() const;
    void setDefaultAngleUnit( AngleUnit angleUnit );

    QFont defaultFont() const;
    void setDefaultFont( const QFont& font );
    
    /**
     * @brief Reload the currently displayed map by reloading texture tiles
     *        from the Internet. In the future this should be extended to all
     *        kinds of data which is used in the map.
     */
    void reload() const;
    
 Q_SIGNALS:
    /**
     * @brief Signal that the zoom has changed, and to what.
     * @param zoom  The new zoom value.
     * @see  zoomView()
     */
    void zoomChanged( int zoom );
    void distanceChanged( const QString& distanceString );

    /**
     * @brief Signal that the theme has changed
     * @param theme  Name of the new theme.
     */
    void themeChanged( const QString& theme );

    void projectionChanged( Projection );

    void mouseMoveGeoPosition( const QString& );

    void mouseClickGeoPosition( qreal lon, qreal lat, GeoDataCoordinates::Unit );

    void framesPerSecond( qreal fps );

    /**
     * This signal is emitted when the repaint of the view was requested.
     * If available with the @p dirtyRegion which is the region the view will change in.
     * If dirtyRegion.isEmpty() returns true, the whole viewport has to be repainted.
     */
    void repaintNeeded( const QRegion& dirtyRegion );

    /**
     * This signal is emitted when the visible region of the map changes. This typically happens
     * when the user moves the map around or zooms.
     */
    void visibleLatLonAltBoxChanged( const GeoDataLatLonAltBox& visibleLatLonAltBox );

 protected:

    /**
     * @brief Enables custom drawing onto the MarbleMap straight after
     * @brief the globe and before all other layers have been rendered.
     * @param painter 
     */
    virtual void customPaint( GeoPainter *painter );

 private:
    Q_DISABLE_COPY( MarbleMap )
    MarbleMapPrivate * const d;
};

}

#endif
