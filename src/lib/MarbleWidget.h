//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef MARBLEWIDGET_H
#define MARBLEWIDGET_H


/** @file
 * This file contains the headers for MarbleWidget.
 *
 * @author Torsten Rahn <tackat@kde.org>
 * @author Inge Wallin  <inge@lysator.liu.se>
 */


#include <QtGui/QWidget>
#include <QtGui/QImage>
#include <QtGui/QPixmap>
#include <QtDesigner/QDesignerExportWidget>

//#include <QtDBus/QtDBus>

#include "GeoPoint.h"
#include "marble_export.h"
#include "MarbleModel.h"
#include "CrossHairFloatItem.h"
#include "CompassFloatItem.h"
#include "MapScaleFloatItem.h"


class QStyleOptionGraphicsItem;
class MarbleWidgetInputHandler;
class MarbleWidgetPopupMenu;
class TextureColorizer;
class MeasureTool;
class TileCreator;
class MarbleWidgetPrivate;
class GpsLayer;
#include "gps/GpxFileModel.h"


/**
 * @short A widget class that displays a view of the earth.
 *
 * This widget displays a view of the earth or any other globe,
 * depending on which dataset is used.  The user can navigate the
 * globe using either a control widget, e.g. the MarbleControlBox, or
 * the mouse.  Only some aspects of the widget can be controlled by
 * the mouse.
 *
 * By clicking on the globe and moving the mouse, the position can be
 * moved.  The user can also zoom by using the scroll wheel of the
 * mouse in the widget. The zoom value is not tied to any units, but
 * is an abstract value without any physical meaning. A value around
 * 1000 shows the full globe in a normal-sized window. Higher zoom
 * values give a more zoomed-in view.
 *
 * The MarbleWidget needs to be provided with a data model to
 * work. This model is contained in the MarbleModel class. The widget
 * can also construct its own model if none is given to the
 * constructor.  This data model contains 3 separate datatypes:
 * <b>tiles</b> which provide the background, <b>vectors</b> which
 * provide things like country borders and coastlines and
 * <b>placemarks</b> which can show points of interest, such as
 * cities, mountain tops or the poles.
 *
 * Except for navigating with the mouse, you can also use it to get
 * information about items on the map. You can either click on a
 * PlaceMark with the left mouse button or with the right mouse button
 * anywhere on the map.
 *
 * The left mouse button opens up a menu with all the PlaceMarks
 * within a certain distance from the mouse pointer. When you choose
 * one item from the menu, Marble will open up a dialog window with
 * some information about the PlaceMark and also try to connect to
 * Wikipedia to retrieve an article about it. If there is such an
 * article, you will get a mini-browser window with the article in a tab.
 *
 * The right mouse button controls a distance tool.  The distance tool
 * is implemented as a menu where you can choose to either create or
 * remove so called Measure Points. Marble will keep track of the
 * Measure Points and show the total distance in the upper left of the
 * widget.  Measure Points are shown on the map as a little white
 * cross.
 *
 * @see MarbleControlBox
 * @see MarbleModel
 */

class MARBLE_EXPORT MarbleWidget : public QWidget
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.marble")

    Q_PROPERTY(int zoom          READ zoom            WRITE zoomView)

    Q_PROPERTY(double longitude  READ centerLongitude WRITE setCenterLongitude)
    Q_PROPERTY(double latitude   READ centerLatitude  WRITE setCenterLatitude)

    Q_PROPERTY(bool showScaleBar READ showScaleBar    WRITE setShowScaleBar)
    Q_PROPERTY(bool showCompass  READ showCompass     WRITE setShowCompass)
    Q_PROPERTY(bool showGrid     READ showGrid        WRITE setShowGrid)
    Q_PROPERTY(bool showPlaces   READ showPlaces      WRITE setShowPlaces)
    Q_PROPERTY(bool showCities   READ showCities      WRITE setShowCities)
    Q_PROPERTY(bool showTerrain  READ showTerrain     WRITE setShowTerrain)

    Q_PROPERTY(bool showRelief   READ showRelief      WRITE setShowRelief)
    Q_PROPERTY(bool showElevationModel READ showElevationModel WRITE setShowElevationModel)

    Q_PROPERTY(bool showIceLayer READ showIceLayer    WRITE setShowIceLayer)
    Q_PROPERTY(bool showBorders  READ showBorders     WRITE setShowBorders)
    Q_PROPERTY(bool showRivers   READ showRivers      WRITE setShowRivers)
    Q_PROPERTY(bool showLakes    READ showLakes       WRITE setShowLakes)

    Q_PROPERTY(bool quickDirty   READ quickDirty      WRITE setQuickDirty)

 public:
    /**
     * @brief Construct a new MarbleWidget.
     * @param parent the parent widget
     *
     * This constructor should be used when you will only use one
     * MarbleWidget.  The widget will create its own MarbleModel when
     * created.
     */
    explicit MarbleWidget( QWidget *parent = 0);

    /**
     * @brief Construct a new MarbleWidget.
     * @param model  the data model for the widget.
     * @param parent the parent widget
     *
     * This constructor should be used when you plan to use more than
     * one MarbleWidget for the same MarbleModel (not yet supported,
     * but will be soon).
     */
    explicit MarbleWidget(MarbleModel *model, QWidget *parent = 0);

    virtual ~MarbleWidget();

    /**
     * @brief Return the model that this view shows.
     */
    MarbleModel  *model() const;

    /**
     * @brief Return the active region in which it's possible to drag the view using the mouse.
     */
    const QRegion  activeRegion();

    /**
     * @brief  Return the radius of the globe in pixels.
     */
    int         radius() const;

    /**
     * @brief  Set the radius of the globe in pixels.
     * @param  radius  The new globe radius value in pixels.
     */
    void        setRadius(const int radius);

    /**
     * @brief Return the current zoom level.
     */
    int         zoom() const;

    /**
     * @brief Return the current distance string.
     */
    QString     distanceString() const;

    /**
     * @brief  Set the minimum value for the zoom.
     * @param  zoom  The new minimum value.
     */
    void        setMinimumZoom( int zoom );

    // FIXME: Apidox
    bool needsUpdate() const;
    void setNeedsUpdate();

    /**
     * @brief Get the screen coordinates corresponding to geographical coordinates in the widget.
     * @param lon    the lon coordinate of the requested pixel position
     * @param lat    the lat coordinate of the requested pixel position
     * @param x      the x coordinate of the pixel is returned through this parameter
     * @param y      the y coordinate of the pixel is returned through this parameter
     * @return @c true  if the geographical coordinates are visible on the screen
     *         @c false if the geographical coordinates are not visible on the screen
     */
    bool screenCoordinates( const double lon, const double lat, 
                            int& x, int& y );

    int northPoleY();
    int northPoleZ();

    /**
     * @brief Get the earth coordinates corresponding to a pixel in the widget.
     * @param x      the x coordinate of the pixel
     * @param y      the y coordinate of the pixel
     * @param lon    the longitude angle is returned through this parameter
     * @param lat    the latitude angle is returned through this parameter
     * @return @c true  if the pixel (x, y) is within the globe
     *         @c false if the pixel (x, y) is outside the globe, i.e. in space.
     */
    bool geoCoordinates( const int x, const int y,
                         double& lon, double& lat,
                         GeoPoint::Unit=GeoPoint::Degree);

    // TODO: APIDOCS
    bool    globalQuaternion( int x, int y, Quaternion &q);

    /**
     * @brief Return the longitude of the center point.
     */
    double  centerLongitude() const;

    /**
     * @brief Return the latitude of the center point.
     */
    double  centerLatitude()  const;

    /**
     * @brief returns the model for all the placemarks on the globe.
     */
    QAbstractListModel *placeMarkModel();

    /**
     * @brief  Return how much the map will move if one of the move slots are called.
     * @return The move step.
     */
    double  moveStep();

    /**
     * @brief  Add a PlaceMark file to the model.
     * @param  filename  the filename of the file containing the PlaceMarks.
     */
    void addPlaceMarkFile( const QString &filename );

    /**
     * @brief  Return the quaternion that specifies the rotation of the globe.
     * @return The quaternion that describes the rotation of the globe.
     */
    Quaternion  planetAxis() const;

    /**
     * @brief  Return a QPixmap with the current contents of the widget.
     */
    QPixmap mapScreenShot();

    /**
     * @brief  Return whether the scale bar is visible.
     * @return The scale bar visibility.
     */
    bool  showScaleBar() const;

    /**
     * @brief  Return whether the compass bar is visible.
     * @return The compass visibility.
     */
    bool  showCompass() const;

    /**
     * @brief  Return whether the coordinate grid is visible.
     * @return The coordinate grid visibility.
     */
    bool  showGrid() const;

    /**
     * @brief  Return whether the place marks are visible.
     * @return The place mark visibility.
     */
    bool  showPlaces() const;

    /**
     * @brief  Return whether the city place marks are visible.
     * @return The city place mark visibility.
     */
    bool  showCities() const;

    /**
     * @brief  Return whether the terrain place marks are visible.
     * @return The terrain place mark visibility.
     */
    bool  showTerrain() const;

    /**
     * @brief  Return whether the relief is visible.
     * @return The relief visibility.
     */
    bool  showRelief() const;

    /**
     * @brief  Return whether the elevation model is visible.
     * @return The elevation model visibility.
     */
    bool  showElevationModel() const;

    /**
     * @brief  Return whether the ice layer is visible.
     * @return The ice layer visibility.
     */
    bool  showIceLayer() const;

    /**
     * @brief  Return whether the borders are visible.
     * @return The border visibility.
     */
    bool  showBorders() const;

    /**
     * @brief  Return whether the rivers are visible.
     * @return The rivers' visibility.
     */
    bool  showRivers() const;

    /**
     * @brief  Return whether the lakes are visible.
     * @return The lakes' visibility.
     */
    bool  showLakes() const;

    /**
     * @brief Return whether Gps Data is visible.
     * @return The Gps Data's visibility.
     */
    bool  showGps() const;

    /**
     * @brief  Return whether quick and dirty rendering is enabled.
     * @return Quick and dirty rendering
     */
    bool  quickDirty() const;

 public Q_SLOTS:

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget);
    /**
     * @brief  Zoom the view to a certain zoomlevel
     * @param  zoom  the new zoom level.
     */
    void  zoomView(int zoom);

    /**
     * @brief  Zoom the view by a certain step
     * @param  zoomStep  the difference between the old zoom and the new
     */
    void  zoomViewBy(int zoomStep);

    /**
     * @brief  Zoom in by the amount zoomStep.
     */
    void  zoomIn();
    /**
     * @brief  Zoom out by the amount zoomStep.
     */
    void  zoomOut();

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
     *
     * This method automatically updates the view
     */
    void  rotateBy( const double &deltaLon, const double &deltaLat );

    /**
     * @brief  Rotate the view by the angle specified by a Quaternion.
     * @param  incRot a quaternion specifying the rotation
     *     *
     * This method automatically updates the view
     */
    void  rotateBy(const Quaternion& incRot);

    /**
     * @brief  Center the view on a geographical point
     * @param  lat  an angle parallel to the latitude lines
     *              +90(N) - -90(S)
     * @param  lon  an angle parallel to the longitude lines
     *              +180(W) - -180(E)
     *
     * This method automatically updates the view
     */
    void  centerOn(const double &lon, const double &lat);

    /**
     * @brief  Center the view on a point
     * @param  index  an index for a QModel, indicating a city
     *
     * This method automatically updates the view
     */
    void  centerOn(const QModelIndex& index);

    /**
     * @brief  Set the latitude for the center point
     * @param  lat  the new value for the latitude
     *
     * This method automatically updates the view
     */
    void setCenterLatitude( double lat );

    /**
     * @brief  Set the longitude for the center point
     * @param  lon  the new value for the longitude
     *
     * This method automatically updates the view
     */
    void setCenterLongitude( double lon );

    /**
     * @brief  Center the view on a point
     * @param  lat  an angle parallel to the latitude lines
     *              +90(N) - -90(S)
     * @param  lon  an angle parallel to the longitude lines
     *              +180(W) - -180(E)
     *
     * This method does NOT automatically update the view
     * and is meant to be used during subsequent transformations 
     */
    void  rotateTo(const double& lon, const double& lat);

    /**
     * @brief  Center the view on a point
     * @param  lat  an angle parallel to the latitude lines
     *              +90(N) - -90(S)
     * @param  lon  an angle parallel to the longitude lines
     *              +180(W) - -180(E)
     * @param  psi  clockwise rotation of the globe
     *              -180(anti-clockwise) - +180(clockwise)
     *
     * This method does NOT automatically update the view
     * and is meant to be used during subsequent transformations 
     */
    void  rotateTo( const double& lon, const double& lat, const double& psi);

    /**
     * @brief  Center the view on a point
     * @param  quat a quaternion specifying the rotation
     *
     * This method does NOT automatically update the view
     * and is meant to be used during subsequent transformations 
     */
    void  rotateTo(const Quaternion& quat);

    /**
     * @brief  Get the Projection used for the map
     * @return @c Spherical         a Globe
     * @return @c Equirectangular   a flat map
     * @return @c Mercator          another flat map
     */
    Projection  projection() const;
    /**
     * @brief  Set the Projection used for the map
     * @param  projection projection type (e.g. Spherical, Equirectangular, Mercator)
     */
    void  setProjection( const Projection projection );

    /**
     * @brief  get the home point
     * @param  lon  the longitude of the home point.
     * @param  lat  the latitude of the home point.
     * @param  zoom the default zoom level of the home point.
     */
    void  home(double &lon, double &lat, int& zoom);
    /**
     * @brief  Set the home point
     * @param  lon  the longitude of the new home point.
     * @param  lat  the latitude of the new home point.
     * @param  zoom the default zoom level for the new home point.
     */
    void  setHome(const double lon, const double lat, const int zoom = 1050);
    /**
     * @brief  Set the home point
     * @param  homePoint  The new home point.
     */
    void  setHome(const GeoPoint& _homePoint, int zoom = 1050);

    /**
     * @brief  Move left by the moveStep.
     */
    void  moveLeft();
    /**
     * @brief  Move right by the moveStep.
     */
    void  moveRight();
    /**
     * @brief  Move up by the moveStep.
     */
    void  moveUp();
    /**
     * @brief  Move down by the moveStep.
     */
    void  moveDown();

    /**
     * @brief Center the view on the default start point with the default zoom.
     */
    void  goHome();

    /**
     * @brief Get the current map theme
     * @param maptheme  The name of the new maptheme.
     */
    QString mapTheme() const;
    /**
     * @brief Set a new map theme
     * @param maptheme  The name of the new maptheme.
     */
    void  setMapTheme( const QString& maptheme );

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
     * @brief Set the current Gps position
     * @param lat the new latitude value
     * @param lon the new longitude value
     */
    void changeCurrentPosition( double lon, double lat );

     /**
     * @brief used to notify about the position of the mouse click
      */
    void notifyMouseClick( int x, int y );

    /**
     * @brief updates the gps tracking point by polling
     */
    void updateGps();

    /**
     * @brief Opens a gpx file for viewing on the Marble Widget
     */
    void openGpxFile( QString &filename );
    
    GpxFileModel * gpxFileModel();

    /**
     * @brief  Set whether for rendering quick and dirty algorithms should be used
     * @param  enabled  Enable quick and dirty rendering
     */
    void setQuickDirty( bool enabled );

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

    void updateChangedMap();
   
    /**
     * @brief update part of the map as defined in the BoundingBox
     */
    void updateRegion( BoundingBox );

 Q_SIGNALS:
    /**
     * @brief Signal that the zoom has changed, and to what.
     */
    void  zoomChanged( int );
    void  distanceChanged( const QString& distanceString );

    void  themeChanged( QString );

    void  mouseMoveGeoPosition( QString );

    void  mouseClickGeoPosition( double lon, double lat, GeoPoint::Unit );

    void  timeout();

 protected:
    /**
     * @brief Reimplementation of the leaveEvent() function in QWidget.
     */
    void  leaveEvent( QEvent *event );

    /**
     * @brief Reimplementation of the paintEvent() function in QWidget.
     */
    void  paintEvent( QPaintEvent *event );

    /**
     * @brief Reimplementation of the resizeEvent() function in QWidget.
     */
    void  resizeEvent( QResizeEvent* );

    void  connectNotify ( const char * signal );
    void  disconnectNotify ( const char * signal );

 private:
    void  construct( QWidget *parent );

    int   toLogScale(int);
    int   fromLogScale(int);

    void  drawAtmosphere();

    void  setActiveRegion();
    
    void  setBoundingBox();

 private:
    MarbleWidgetPrivate  * const d;
};


#endif // MARBLEWIDGET_H
