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

class KAtlasView : public QWidget
{
    Q_OBJECT

public:
	KAtlasView(QWidget *parent = 0);

	const QRegion getActiveRegion();
	bool getGlobeSphericals(int x, int y, float& alpha, float& beta);
	QAbstractListModel* getPlaceMarkModel(){ return m_pGlobe->getPlaceMarkModel(); };

	float getMoveStep();
	void setMinimumZoom( int zoom ){ minimumzoom = zoom; }

	void addPlaceMarkFile( QString filename ){ m_pGlobe->addPlaceMarkFile( filename ); }

	KAtlasGlobe* globe() const { return m_pGlobe; }

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
   		m_cachedSize = QSize();
	}

signals:
	void zoomChanged(int);

private:
	int toLogScale(int);
	int fromLogScale(int);
	int m_logzoom;
	int m_zoomStep;
	
	int minimumzoom;	

	KAtlasGlobe* m_pGlobe;

	KAtlasViewInputHandler* inputhandler;
	KAtlasViewPopupMenu* m_popupmenu;

	TextureColorizer* sealegend;
	QImage* m_pCanvasImage;

	KAtlasCrossHair m_crosshair;
	KAtlasMapScale m_mapscale;
	KAtlasWindRose m_windrose;
	MeasureTool* m_pMeasureTool;

	QRegion activeRegion;
	void setActiveRegion();

	QSize m_cachedSize;
	QPixmap m_cachedPixmap;

protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent (QResizeEvent*);
};

#endif // KATLASVIEW_H
