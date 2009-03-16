//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Carlos Licea     <carlos _licea@hotmail.com>
//


#ifndef GPSLAYER_H
#define GPSLAYER_H

#include "AbstractLayer.h"

#include "config-libgps.h" // krazy:exclude=includes

#ifdef HAVE_LIBGPS
#include <libgpsmm.h>
#endif

class QRegion;

namespace Marble
{

class GpsTracking;
class GpxFile;
class GpxFileModel;
class Track;
class Waypoint;

class GpsLayer : public AbstractLayer
{
 public:
    explicit GpsLayer( GpxFileModel *fileModel, QObject *parent =0 );
    ~GpsLayer();
    virtual void paintLayer( ClipPainter *painter, 
                             const QSize &canvasSize,
                             ViewParams *viewParams );
    virtual void paintCurrentPosition( ClipPainter *painter, 
                                       const QSize &canvasSize, 
                                       ViewParams *viewParams );

    void changeCurrentPosition( qreal lat, qreal lon );

    void loadGpx(const QString &fileName);
//  public slots:
    bool updateGps(const QSize &canvasSize, ViewParams *viewParams,
                   QRegion &reg);

    GpxFileModel        *m_fileModel;
 private:
    Waypoint            *m_currentPosition;

//     QVector<GpxFile*>   *m_files;

    GpxFile             *m_currentGpx;
    GpsTracking         *m_tracking;
};

}

#endif
