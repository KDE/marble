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


// This is the main widget where the map is painted.

class MarbleWidget : public QWidget
{
    Q_OBJECT

 public:
    explicit MarbleWidget(KAtlasGlobe *globe, QWidget *parent = 0);

    // The model this view shows.
    KAtlasGlobe* globe() const { return m_pGlobe; }

    const QRegion  activeRegion();

    bool                globeSphericals( int x, int y, 
                                         float& alpha, float& beta );
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
