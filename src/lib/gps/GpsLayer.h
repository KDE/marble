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


#include "AbstractLayer/AbstractLayer.h"

#include "config-libgps.h" // krazy:exclude=includes

#ifdef HAVE_LIBGPS
#include <libgpsmm.h>
#endif


class Waypoint;
class Track;
class BoundingBox;
class GpxFile;
class QRegion;
class GpsTracking;
#include "GpxFileModel.h"


class GpsLayer : public AbstractLayer
{
 public:
    explicit GpsLayer( GpxFileModel *fileModel, QObject *parent =0 );
    ~GpsLayer();
    virtual void paintLayer( ClipPainter *painter, 
                             const QSize &canvasSize,
                             ViewParams *viewParams,
                             BoundingBox &box );
    virtual void paintCurrentPosition( ClipPainter *painter, 
                                       const QSize &canvasSize, 
                                       ViewParams *viewParams );

    void changeCurrentPosition( double lat, double lon );

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

#endif
