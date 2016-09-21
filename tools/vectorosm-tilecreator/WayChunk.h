//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Akshat Tandon <akshat.tandon@research.iiit.ac.in>
//

#ifndef MARBLE_WAYCHUNK_H
#define MARBLE_WAYCHUNK_H

#include "GeoDataFeature.h"

#include <QList>

namespace Marble {

class GeoDataPlacemark;

class WayChunk
{
public:
    WayChunk(GeoDataPlacemark *placemark, qint64 first, qint64 last );
    ~WayChunk();
    void append(GeoDataPlacemark *placemark, qint64 last);
    void append(WayChunk *chunk);
    void prepend(GeoDataPlacemark *placemark, qint64 first);

    /*
     * Creates a new placemark object by concatenating all the linsetrings which exist in the WayChunk
     * Caller has the responsibility of deleting the object.
     */
    GeoDataPlacemark* merge();

    qint64 first() const;
    qint64 last() const;
    void reverse();
    qint64 id() const;
    void printIds() const;
    int size() const;
    bool concatPossible(GeoDataPlacemark *placemark) const;
    GeoDataFeature::GeoDataVisualCategory visualCategory() const;

private:
    QList<GeoDataPlacemark*> m_wayList;
    qint64 m_first;
    qint64 m_last;
    GeoDataFeature::GeoDataVisualCategory  m_visualCategory;
};

}

#endif
