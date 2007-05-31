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

#include "katlasglobe.h"
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
 * depending on which dataset is used.  The user can navigate on the
 * earth using either a control widget, e.g. the MarbleControlBox or
 * the mouse.  Only some aspects of the widget can be controlled by
 * the mouse.
 *
 * By clicking on the globe and moving the mouse, the position can be
 * moved.  The user can also zoom by using the scroll wheel of the
 * mouse in the widget.
 *
 * The user can control all aspects of it by using a MarbleControlBox
 * widget.
 *
 * To work, it needs to be provided with a data model, which is
 * contained in a KAtlasGlobe class.  This data model contains 3
 * separate datatypes: <b>tiles</b> which provide the background,
 * <b>vectors</b> which provide things like country borders and
 * coastlines and <b>placemarks</b> which can show points of interest,
 * such as cities, mountain tops or the poles.
 *
 * @see MarbleControlBox
 * @see KAtlasGlobe
 */

class MarbleWidget : public QWidget
{
    Q_OBJECT

 public:
    /**
     * @brief Construct a new MarbleWidget.
     * @param globe  the data model for the widget.
     * @param parent the parent widget
     */
    explicit MarbleWidget(KAtlasGlobe *globe, QWidget *parent = 0);

    /**
     * @brief The model this view shows.
     */
    KAtlasGlobe   *globe() const { return m_pGlobe; }

    /**
     * @brief Return the active region.
     */
    const QRegion  activeRegion();

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
        return m_pGlobe->getPlaceMarkModel();
    }

    float  moveStep();

    void   setMinimumZoom( int zoom ){ m_minimumzoom = zoom; }

    void addPlaceMarkFile( QString filename ){ m_pGlobe->addPlaceMarkFile( filename ); }

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
	m_pGlobe->setMapTheme( maptheme );
		
	// FIXME: Force update...
    }

    void creatingTilesStart( const QString& name, const QString& description );
    void creatingTilesProgress( int progress );

 signals:
    void  zoomChanged(int);

 protected:
    void  paintEvent(QPaintEvent *event);
    void  resizeEvent(QResizeEvent*);

 private:
    int   toLogScale(int);
    int   fromLogScale(int);

    void  setActiveRegion();

 private:
    // The model we are showing.
    KAtlasGlobe  *m_pGlobe;

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
