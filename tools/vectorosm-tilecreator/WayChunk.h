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

#include "GeoDataPlacemark.h"

#include <QList>
#include <QSharedPointer>

namespace Marble {

class GeoDataPlacemark;

class WayChunk
{
private:
    typedef QSharedPointer<GeoDataPlacemark> PlacemarkPtr;

public:
    typedef QSharedPointer<WayChunk> Ptr;

    WayChunk(const PlacemarkPtr &placemark, qint64 first, qint64 last );
    ~WayChunk();
    void append(const PlacemarkPtr &placemark, qint64 last);
    void append(const Ptr &chunk);
    void prepend(const PlacemarkPtr & placemark, qint64 first);

    /*
     * Creates a new placemark object by concatenating all the linsetrings which exist in the WayChunk
     * Caller has the responsibility of deleting the object.
     */
    PlacemarkPtr merge();

    qint64 first() const;
    qint64 last() const;
    void reverse();
    int size() const;
    bool concatPossible(const GeoDataPlacemark &placemark) const;

private:
    QVector<PlacemarkPtr> m_wayList;
    qint64 m_first;
    qint64 m_last;
    GeoDataPlacemark::GeoDataVisualCategory m_visualCategory;
};

}

#endif
