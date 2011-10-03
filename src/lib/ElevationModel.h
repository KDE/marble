//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
//


#ifndef MARBLE_ELEVATIONMODEL_H
#define MARBLE_ELEVATIONMODEL_H

#include "GeoDataCoordinates.h"
#include "marble_export.h"
#include "TileId.h"

#include <QtCore/QObject>
#include <QtCore/QCache>
#include <QtGui/QImage>

namespace Marble
{

class MarbleModel;
class ElevationModelPrivate;

class MARBLE_EXPORT ElevationModel : public QObject
{
    Q_OBJECT
public:
    ElevationModel( MarbleModel * const model );

    qreal height( qreal lon, qreal lat ) const;
    QList<GeoDataCoordinates> heightProfile( qreal fromLon, qreal fromLat, qreal toLon, qreal toLat ) const;

Q_SIGNALS:
    /**
     * Elevation tiles loaded. You will get more accurate results when querying height
     * for at least one that was queried before.
     **/
    void updateAvailable();

private:
    friend class ElevationModelPrivate;
    ElevationModelPrivate *d;
};

}

#endif // MARBLE_ELEVATIONMODEL_H
