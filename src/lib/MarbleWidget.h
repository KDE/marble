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
#


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

#include "marble_export.h"
#include "MarbleModel.h"
#include "katlascrosshair.h"
#include "katlasmapscale.h"
#include "katlaswindrose.h"


class KAtlasViewInputHandler;
class KAtlasViewPopupMenu;
class TextureColorizer;
class MeasureTool;
class KAtlasTileCreatorDialog;
class MarbleWidgetPrivate;
class GpsLayer;


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

    Q_PROPERTY(int zoom READ zoom WRITE zoomView)

    Q_PROPERTY(double longitude  READ centerLongitude WRITE setCenterLongitude)
    Q_PROPERTY(double latitude   READ centerLatitude  WRITE setCenterLatitude)

    Q_PROPERTY(bool showScaleBar READ showScaleBar    WRITE setShowScaleBar)
    Q_PROPERTY(bool showWindRose READ showWindRose    WRITE setShowWindRose)
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
    explicit MarbleWidget(QWidget *parent = 0);

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

    /**
     * @brief Return the model that this view shows.
     */
    MarbleModel  *model() const;

    /**
     * @brief Return the active region.
     */
    const QRegion  activeRegion();

    /**
     * @brief Return the current zoom level.
     */
    int  zoom() const;

    double  centerLatitude();
    double  centerLongitude();


    /**
     * @brief Get the earth coordinates corresponding to a pixel in the widget.
     * @param x      the x coordinate of the pixel
     * @param y      the y coordinate of the pixel
     * @param alpha  the alpha angle is returned through this parameter
     * @param beta   the beta angle is returned through this parameter
     * @return @c true  if the pixel (x, y) is within the globe
     *         @c false if the pixel (x, y) is outside the globe, i.e. in space.
     */
    bool            globeSphericals( int x, int y, 
                                     double& alpha, double& beta );
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
     * @brief  Set the minimum value for the zoom.
     * @param  zoom  The new minimum value.
     */
    void   setMinimumZoom( int zoom );

    /**
     * @brief  Add a PlaceMark file to the model.
     * @param  filename  the filename of the file containing the PlaceMarks.
     */
    void addPlaceMarkFile( const QString &filename );

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
     * @brief  Return whether the scale bar is visible.
     * @return The scale bar visibility.
     */
    bool  showWindRose() const;

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
     * @brief  Zoom in by the zoomStep.
     */
    void  zoomIn();
    /**
     * @brief  Zoom out by the zoomStep.
     */
    void  zoomOut();

    /**
     * @brief  Rotate the view by the two angles phi and theta.
     * @param  phi    an angle loosely equivalent to the latitude
     * @param  theta  an angle loosely equivalent to the longitude
     *
     * This function rotates the view by two angles, phi and theta.
     * If we start on (0, 0), the result will be the exact equivalent
     * of (lat, lon), otherwise the resulting angle will be the sum of
     * the previous position and the two offsets.
     */
    void  rotateBy(const double &phi, const double &theta);
    /**
     * @brief  Center the view on a point
     * @param  lat  an angle parallel to the latitude lines
     * @param  lon  an angle parallel to the longitude lines
     */
    void  centerOn(const double &lat, const double &lon);
    /**
     * @brief  Center the view on a point
     * @param  index  an index for a QModel, indicating a city
     */
    void  centerOn(const QModelIndex& index);

    /**
     * @brief  Set the latitude for the centerPoint
     * @param  lat  the new value for the latitude
     */
    void setCenterLatitude( double lat );

    /**
     * @brief  Set the longitude for the centerPoint
     * @param  lng  the new value for the longitude
     */
    void setCenterLongitude( double lng );

    /**
     * @brief  Set the home point
     * @param  lon  the longitude of the new home point.
     * @param  lat  the latitude of the new home point.
     */
    void  setHome(const double &lon, const double &lat, int zoom = 1050);
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
     * @brief  Set whether the wind rose overlay is visible
     * @param  visible  visibility of the wind rose
     */
    void setShowWindRose( bool visible );

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
    void changeGpsPosition( double lat, double lon);
    
     /**
     * @brief used to notify about the position of the mouse click
      */
    void gpsCoordinatesClick( int, int );
    
    /**
     * @brief updates the gps tracking point by polling
     */
    void updateGps();

    /**
     * @brief  Set whether for rendering quick and dirty algorithms should be used 
     * @param  enabled  Enable quick and dirty rendering
     */
    void setQuickDirty( bool enabled );

    /**
     * @brief A slot that is called when the model starts to create new tiles.
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
    void creatingTilesStart( const QString& name, const QString& description );

    /**
     * @brief A slot that is called while the model is creating new tiles.
     * @param  progress  the percentage done
     * @see    creatingTilesStart
     *
     * This function is connected to the models' signal with the same
     * name. See creatingTilesStart for a description of how it is
     * used.
     */
    void creatingTilesProgress( int progress );

 Q_SIGNALS:
    /**
     * @brief Signal that the zoom has changed, and to what.
     */
    void  zoomChanged( int );

    void  mouseGeoPosition( QString );
    
    void gpsClickPos( double, double, GeoPoint::Unit );

    void timeout(); // we need a better name here!

 protected:
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

    void  setActiveRegion();

 private:
    MarbleWidgetPrivate  * const d; 
};


#endif // MARBLEWIDGET_H
