//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
//


#ifndef MARBLE_ELEVATIONMODEL_H
#define MARBLE_ELEVATIONMODEL_H

#include "marble_export.h"

#include <QObject>

class QImage;

namespace Marble
{
class GeoDataCoordinates;

namespace {
    unsigned int const invalidElevationData = 32768;
}

class TileId;
class ElevationModelPrivate;
class HttpDownloadManager;
class PluginManager;

class MARBLE_EXPORT ElevationModel : public QObject
{
    Q_OBJECT
public:
    explicit ElevationModel( HttpDownloadManager *downloadManager, PluginManager* pluginManager, QObject *parent = 0 );
    ~ElevationModel();

    qreal height( qreal lon, qreal lat ) const;
    QVector<GeoDataCoordinates> heightProfile( qreal fromLon, qreal fromLat, qreal toLon, qreal toLat ) const;

Q_SIGNALS:
    /**
     * Elevation tiles loaded. You will get more accurate results when querying height
     * for at least one that was queried before.
     **/
    void updateAvailable();

private:
    Q_PRIVATE_SLOT( d, void tileCompleted( const TileId&, const QImage& ) )

private:
    friend class ElevationModelPrivate;
    ElevationModelPrivate *d;
};

}

#endif // MARBLE_ELEVATIONMODEL_H
