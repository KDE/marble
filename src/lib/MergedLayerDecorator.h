// Copyright 2008 David Roberts
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.

#ifndef MERGED_LAYER_DECORATOR_H
#define MERGED_LAYER_DECORATOR_H

#include <QImage>
#include <QObject>

#include "SunLocator.h"

class MergedLayerDecorator : public QObject
{
    Q_OBJECT
	
 public:
    explicit MergedLayerDecorator(SunLocator* sunLocator);
    virtual ~MergedLayerDecorator();
	
    void paint(const QString& theme);
    void paintTileId(const QString& theme);
    
    void showClouds(bool show) {m_cloudlayer = show;}
    void showTileId(bool show) {m_showTileId = show;}
	
    void setTile(QImage* tile);
    void setInfo(int x, int y, int level, int id);
	
 Q_SIGNALS:
    void downloadTile(const QString& relativeUrlString, const QString& id);
    void repaintMap();
	
 private:
    QImage loadRawTile(const QString& theme);
    int maxDivisor( int maximum, int fullLength );
    void paintSunShading();
    void paintClouds();
	
 protected:
    QImage* m_tile;
    int m_x;
    int m_y;
    int m_level;
    int m_id;
    SunLocator* m_sunLocator;
    bool m_cloudlayer;
    bool m_showTileId;
};

#endif
