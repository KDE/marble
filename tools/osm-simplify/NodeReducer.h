//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Akshat Tandon <akshat.tandon@research.iiit.ac.in>
//

#ifndef MARBLE_NODEREDUCER_H
#define MARBLE_NODEREDUCER_H

#include "PlacemarkFilter.h"
#include "GeoDataLineString.h"

class NodeReducer : public PlacemarkFilter {
public:
    NodeReducer(GeoDataDocument* document, int zoomLevel);
    virtual void process();

private:
    GeoDataLineString* reduce(GeoDataLineString* lineString);
    static qreal resolutionForLevel(int level);

    qreal m_resolution;
    qint64 m_count;
};

#endif
