// (c) 2008 David Roberts

#ifndef MERGEDLAYERPAINTER_H
#define MERGEDLAYERPAINTER_H

#include <QImage>
#include <QObject>

#include "SunLocator.h"

class MergedLayerPainter : public QObject {
	Q_OBJECT
	
	public:
	explicit MergedLayerPainter();
	virtual ~MergedLayerPainter();
	
	void paint(const QString& theme);
	void paintClouds();
	void paintSunShading(SunLocator* sunLocator);
	void paintTileId(const QString& theme);
	
	void setTile(QImage* tile);
	void setInfo(int x, int y, int level, int id);
	
	Q_SIGNALS:
	void downloadTile(const QString& relativeUrlString, const QString& id);
	
	private:
	QImage loadRawTile(const QString& theme);
	
	protected:
	QImage* m_tile;
	int m_x;
	int m_y;
	int m_level;
	int m_id;
};

#endif
// (c) 2008 David Roberts

#ifndef MERGEDLAYERPAINTER_H
#define MERGEDLAYERPAINTER_H

#include <QImage>
#include <QObject>

#include "SunLocator.h"

class MergedLayerPainter : public QObject {
	Q_OBJECT
	
	public:
	explicit MergedLayerPainter();
	virtual ~MergedLayerPainter();
	
	void paint(const QString& theme);
	void paintClouds();
	void paintSunShading(SunLocator* sunLocator);
	void paintTileId(const QString& theme);
	
	void setTile(QImage* tile);
	void setInfo(int x, int y, int level, int id);
	
	Q_SIGNALS:
	void downloadTile(const QString& relativeUrlString, const QString& id);
	
	private:
	QImage loadRawTile(const QString& theme);
	
	protected:
	QImage* m_tile;
	int m_x;
	int m_y;
	int m_level;
	int m_id;
};

#endif
