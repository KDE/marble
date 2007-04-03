#ifndef KATLASVIEW_H
#define KATLASVIEW_H

#include <QtGui/QWidget>
#include <QtGui/QImage>

#include "katlasglobe.h"
#include "katlascrosshair.h"
#include "katlasmapscale.h"
#include "katlaswindrose.h"

class KAtlasViewInputHandler;
class KAtlasViewPopupMenu;
class TextureColorizer;
class QAbstractItemModel;
class MeasureTool;

// This is the main widget where the map is painted.

class KAtlasView : public QWidget
{
    Q_OBJECT

 public:
    KAtlasView(QWidget *parent = 0);

    // The model this view shows.
    KAtlasGlobe* globe() const { return m_pGlobe; }

    const QRegion getActiveRegion();
    bool getGlobeSphericals(int x, int y, float& alpha, float& beta);
    QAbstractListModel* getPlaceMarkModel(){ return m_pGlobe->getPlaceMarkModel(); };

    float getMoveStep();
    void setMinimumZoom( int zoom ){ minimumzoom = zoom; }

    void addPlaceMarkFile( QString filename ){ m_pGlobe->addPlaceMarkFile( filename ); }

 public slots:
    void zoomView(int);

    void zoomViewBy(int);
    void zoomIn();
    void zoomOut();

    void rotateBy(const float&, const float&);
    void centerOn(const float&, const float&);
    void centerOn(const QModelIndex&);

    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();

    void goHome();

    void setMapTheme( const QString& maptheme )
    {
	m_pGlobe->setMapTheme( maptheme );
		
	// Force update...
    }

 signals:
    void zoomChanged(int);

 protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent (QResizeEvent*);

 private:
    int toLogScale(int);
    int fromLogScale(int);

 private:
    // The model we are showing.
    KAtlasGlobe  *m_pGlobe;

    int m_logzoom;
	
    int m_zoomStep;
    int minimumzoom;	


    KAtlasViewInputHandler* inputhandler;
    KAtlasViewPopupMenu* m_popupmenu;

    TextureColorizer* sealegend;
    QImage* m_pCanvasImage;

    // Parts of the image
    KAtlasCrossHair  m_crosshair;
    KAtlasMapScale   m_mapscale; // Shown in the lower left
    KAtlasWindRose   m_windrose; // Shown in the upper right

    // Tools
    MeasureTool* m_pMeasureTool;

    QRegion activeRegion;
    void setActiveRegion();

    QPixmap m_cachedPixmap;
};

#endif // KATLASVIEW_H
