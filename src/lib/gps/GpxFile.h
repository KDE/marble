//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson  <g.real.ate@gmail.com>
//
#ifndef GPXFILE_H
#define GPXFILE_H

#include "AbstractLayer/AbstractLayerInterface.h"

#include <QtCore/Qt>
#include <QtCore/QString>

class ClipPainter;
class QPoint;
class QSize;
class Quaternion;
class BoundingBox;
class Waypoint;
class Track;
class Route;
class WaypointContainer;
class TrackContainer;
class RouteContainer;

class GpxFile : public AbstractLayerInterface
{
 public:
    GpxFile();
    
    virtual void draw( ClipPainter *painter, const QPoint &point );
    
    virtual void draw( ClipPainter *painter, 
                       const QSize &canvasSize, double radius,
                       Quaternion invRotAxis );
    virtual void draw( ClipPainter *painter, 
                       const QSize &canvasSize, double radius,
                       Quaternion invRotAxis, BoundingBox box );
    void addWaypoint( Waypoint *waypoint );
    void addTrack( Track *track );
    void addRoute( Route *route );
    void    setName( const QString &name );
    
    Qt::ItemFlags   flags();
    QString         display();
    
 private:
    QString             m_name;
    WaypointContainer  *m_waypoints;
    TrackContainer      *m_tracks;
    RouteContainer      *m_routes;
};

#endif

