#ifndef KATLASVIEW_H
#define KATLASVIEW_H

#include <QWidget>
#include <QImage>
#include "katlasglobe.h"
#include "katlascrosshair.h"
#include "katlasmapscale.h"
#include "katlaswindrose.h"

class KAtlasViewInputHandler;
class TextureColorizer;
class QAbstractItemModel;

class KAtlasView : public QWidget
{
    Q_OBJECT

public:
	KAtlasView(QWidget *parent = 0);

	const QRegion getActiveRegion();
	bool getGlobeSphericals(int, int, float&, float&);
	QAbstractListModel* getPlaceMarkModel(){ return globe->getPlaceMarkModel(); };

	float getMoveStep();
	void setMinimumZoom( int zoom ){ minimumzoom = zoom; }

	void addPlaceMarkFile( QString filename ){ globe->addPlaceMarkFile( filename ); }

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
		globe->setMapTheme( maptheme );
		
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

	KAtlasGlobe* globe;
	KAtlasViewInputHandler* inputhandler;
	TextureColorizer* sealegend;
	QImage canvasimg;

	KAtlasCrossHair m_crosshair;
	KAtlasMapScale m_mapscale;
	KAtlasWindRose m_windrose;

	QRegion activeRegion;
	void setActiveRegion();

	QSize m_cachedSize;
	QPixmap m_cachedPixmap;

protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent (QResizeEvent*);
};

#endif // KATLASVIEW_H
