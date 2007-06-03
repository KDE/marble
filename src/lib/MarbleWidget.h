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

#include "marble_export.h"
#include "MarbleModel.h"
#include "katlascrosshair.h"
#include "katlasmapscale.h"
#include "katlaswindrose.h"


class KAtlasViewInputHandler;
class KAtlasViewPopupMenu;
class TextureColorizer;
class QAbstractItemModel;
class MeasureTool;
class KAtlasTileCreatorDialog;


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
 * values gives a more zoomed-in view.
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
 * Wikipedia to retreive an article about it. If there is such an
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
    MarbleModel   *model() const { return m_model; }

    /**
     * @brief Return the active region.
     */
    const QRegion  activeRegion();

    /**
     * @brief Return the current zoom level.
     */
    int  zoom() const { return m_logzoom; }

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
                                     float& alpha, float& beta );
    /**
     * @brief returns the model for all the placemarks on the globe.
     */
    QAbstractListModel *placeMarkModel(){
        return m_model->getPlaceMarkModel();
    }

    /**
     * @brief  Return how much the map will move if one of the move slots are called.
     * @return The move step.
     */
    float  moveStep();

    /**
     * @brief  Set the minimum value for the zoom.
     * @param  zoom  The new minimum value.
     */
    void   setMinimumZoom( int zoom ){ m_minimumzoom = zoom; }

    /**
     * @brief  Add a PlaceMark file to the model.
     * @param  filename  the filename of the file containing the PlaceMarks.
     */
    void addPlaceMarkFile( QString filename ){ m_model->addPlaceMarkFile( filename ); }

    /**
     * @brief  Return a QPixmap with the current contents of the widget.
     */
    QPixmap mapScreenShot(){ return QPixmap::grabWidget( this ); }

 public slots:
    void  zoomView(int);

    void  zoomViewBy(int);
    void  zoomIn();
    void  zoomOut();

    void  rotateBy(const float&, const float&);
    void  centerOn(const float&, const float&);
    void  centerOn(const QModelIndex&);

    void  moveLeft();
    void  moveRight();
    void  moveUp();
    void  moveDown();

    void  goHome();

    void  setMapTheme( const QString& maptheme )
    {
	m_model->setMapTheme( maptheme );
		
	// FIXME: Force update...
    }

    void creatingTilesStart( const QString& name, const QString& description );
    void creatingTilesProgress( int progress );

 signals:

    /**
     * @brief Signal that the zoom has changed, and to what.
     */
    void  zoomChanged(int);

 protected:
    /**
     * @brief Reimplementation of the paintEvent() function in QWidget.
     */
    void  paintEvent(QPaintEvent *event);

    /**
     * @brief Reimplementation of the resizeEvent() function in QWidget.
     */
    void  resizeEvent(QResizeEvent*);

 private:
    void  construct(QWidget *parent);

    int   toLogScale(int);
    int   fromLogScale(int);

    void  setActiveRegion();

 private:
    // The model we are showing.
    MarbleModel  *m_model;

    int           m_logzoom;
	
    int           m_zoomStep;
    int           m_minimumzoom;    
    int           m_maximumzoom;

    KAtlasViewInputHandler  *m_inputhandler;
    KAtlasViewPopupMenu     *m_popupmenu;

    TextureColorizer        *m_sealegend;
    QImage                  *m_pCanvasImage;

    // Parts of the image
    KAtlasCrossHair  m_crosshair;
    KAtlasMapScale   m_mapscale; // Shown in the lower left
    KAtlasWindRose   m_windrose; // Shown in the upper right

    // Tools
    MeasureTool  *m_pMeasureTool;

    QRegion       m_activeRegion;

    QPixmap       m_cachedPixmap;

    // The progress dialog for the tile creator.
    KAtlasTileCreatorDialog  *m_tileCreatorDlg;
};


#endif // MARBLEWIDGET_H
