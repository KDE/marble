/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) <year>  <name of author>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef MARBLE_ALTITUDEMODEL_H
#define MARBLE_ALTITUDEMODEL_H

#include <QObject>

#include "marble_export.h"

namespace Marble {

class MarbleModel;
class HttpDownloadManager;
class GeoSceneGroup;
class GeoSceneTexture;
class MapThemeManager;
class TileLoader;


class MARBLE_EXPORT AltitudeModel : public QObject
{
    Q_OBJECT
public:
    AltitudeModel( MapThemeManager const * const mapThemeManager,
                        HttpDownloadManager * const downloadManager, MarbleModel * const model );

    qreal height(qreal lat, qreal lon);


private:
    void updateTextureLayers();

private: //TODO d pointer
    TileLoader *m_tileLoader;
    const MapThemeManager* m_mapThemeManager;
    const GeoSceneTexture *m_textureLayer;
    MarbleModel *m_model;
};

}

#endif // MARBLE_ALTITUDEMODEL_H
