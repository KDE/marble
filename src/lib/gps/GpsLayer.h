//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Carlos Licea     <carlos _licea@hotmail.com>
//


#ifndef MARBLE_GPSLAYER_H
#define MARBLE_GPSLAYER_H

#include "AbstractLayer.h"

class QRegion;

namespace Marble
{

class PositionTracking;
class GpxFile;
class GpxFileModel;
class PluginManager;
class Track;
class Waypoint;

class GpsLayer : public AbstractLayer
{

 public:
    explicit GpsLayer( GpxFileModel *fileModel,
                       PositionTracking *positionTracking,
                       QObject *parent =0 );
    ~GpsLayer();
    virtual void paintLayer( ClipPainter *painter, 
                             const QSize &canvasSize,
                             ViewParams *viewParams );

    GpxFileModel        *m_fileModel;
public slots:
    virtual void clearModel();
 private:
    PositionTracking         *m_tracking;
};

}

#endif
