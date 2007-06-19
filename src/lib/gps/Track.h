//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//
#ifndef TRACK_H
#define TRACK_H

#include "AbstractLayer/AbstractLayerData.h"
#include "GpsElement.h"
#include "WaypointContainer.h"

/*! \brief representation of a GPS Track
 * 
 * A GPS track is an ordered list of points describing a path. These
 * points usually represent where someone has previously traveled
 * and the points have been recorded by the device.
 * 
 * Each representation of a Track (as set out in the GPS schema) can
 * contain an infinite amount of track segments. 
 * 
 * Quoted from http://www.topografix.com/gpx.asp :
 * A Track Segment holds a list of Track Points which are logically
 * connected in order. To represent a single GPS track where GPS
 * reception was lost, or the GPS receiver was turned off, start a new
 * Track Segment for each continuous span of track data.
 */
class Track : public AbstractLayerData, public GpsElement
{
 public:
    //!empty constructor
    Track();
    virtual ~Track();

 private:
    //!collection of Track Segments
    QVector<WaypointContainer> *m_trackSegs;
};

#endif //TRACK_H
