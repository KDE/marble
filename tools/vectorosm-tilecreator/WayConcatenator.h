//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Akshat Tandon <akshat.tandon@research.iiit.ac.in>
//

#ifndef MARBLE_WAYCONCATENATOR_H
#define MARBLE_WAYCONCATENATOR_H

#include "BaseFilter.h"
#include "TagsFilter.h"

namespace Marble {

class WayChunk;

class WayConcatenator: public BaseFilter
{
private:
    typedef QSharedPointer<GeoDataPlacemark> PlacemarkPtr;

public:
    WayConcatenator(GeoDataDocument *document);
    ~WayConcatenator();

    int originalWays() const;
    int mergedWays() const;

private:
    void createWayChunk(const PlacemarkPtr &placemark, qint64 firstId, qint64 lastId);
    WayChunk* wayChunk(const PlacemarkPtr &placemark, qint64 matchId) const;
    void concatFirst(const PlacemarkPtr &placemark, WayChunk *chunk);
    void concatLast(const PlacemarkPtr & placemark, WayChunk *chunk);
    void concatBoth(const PlacemarkPtr &placemark, WayChunk *chunk, WayChunk *otherChunk);
    void addWayChunks();
    void prepareDocument();

    QMultiHash<qint64, WayChunk*> m_hash;
    QVector<WayChunk*> m_chunks;
    QVector<PlacemarkPtr> m_wayPlacemarks;
    QVector<GeoDataPlacemark*> m_otherPlacemarks;

    int m_originalWays;
    int m_mergedWays;
};

}

#endif
