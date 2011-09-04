//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
//


#ifndef MARBLE_ALTITUDEMODEL_H
#define MARBLE_ALTITUDEMODEL_H

#include <QObject>
#include <QCache>
#include <QImage>

#include "marble_export.h"
#include "TileId.h"
#include <GeoDataCoordinates.h>

namespace Marble {

class MarbleModel;
class HttpDownloadManager;
class GeoSceneGroup;
class GeoSceneTexture;
class MapThemeManager;
class TileLoader;
class AltitudeModelPrivate;

class MARBLE_EXPORT AltitudeModel : public QObject
{
    Q_OBJECT
public:
    AltitudeModel( MarbleModel * const model );

    qreal height(qreal lat, qreal lon);
    QList<GeoDataCoordinates> heightProfile( qreal fromLat, qreal fromLon, qreal toLat, qreal toLon );

Q_SIGNALS:
    /**
     * Altitude tiles loaded. You will get more acurate results when quering height
     * for at least one that was queried before.
     **/
    void loadCompleted();

private:
    friend class AltitudeModelPrivate;
    AltitudeModelPrivate *d;
};

}

#endif // MARBLE_ALTITUDEMODEL_H
