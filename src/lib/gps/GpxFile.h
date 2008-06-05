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

#include <QtCore/QString>

#include "AbstractLayerInterface.h"

class QTextStream;
class QPoint;
class QSize;

class ClipPainter;
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
    explicit GpxFile( const QString &fileName );
    GpxFile();

    ~GpxFile();

    virtual void draw( ClipPainter *painter, const QPoint &point );

    virtual void draw( ClipPainter *painter, 
                       const QSize &canvasSize,
                       ViewParams *viewParams );
    virtual void draw( ClipPainter *painter, 
                       const QSize &canvasSize, ViewParams *viewParams,
                       const BoundingBox &box );
    virtual void printToStream( QTextStream & ) const;

    void addWaypoint( Waypoint *waypoint );
    void addTrack( Track *track );
    void addRoute( Route *route );
    void    setName( const QString &name );

    Qt::ItemFlags   flags() const;
    QString         display();
    Qt::CheckState  checkState();
    void            setCheckState( Qt::CheckState state );
    void            setCheckState( bool state );
    bool            active() const;
    void            setActive( bool active );

 private:
    bool                m_active;
    QString             m_name;
    WaypointContainer  *m_waypoints;
    TrackContainer      *m_tracks;
    RouteContainer      *m_routes;
    Qt::CheckState      m_checkState;
};

#endif
