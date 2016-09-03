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

#include "PlacemarkFilter.h"
#include "TagsFilter.h"

namespace Marble
{
class WayChunk;

class WayConcatenator : public TagsFilter
{
public:
    WayConcatenator(GeoDataDocument *document, const QStringList &tagsList, bool andFlag = false);
    virtual void process();
    ~WayConcatenator();
private:
    QMultiHash<qint64, WayChunk*> m_hash;
    QVector<WayChunk*> m_chunks;
    QVector<GeoDataPlacemark> m_wayPlacemarks;
    void createWayChunk(GeoDataPlacemark *placemark, qint64 firstId, qint64 lastId);
    WayChunk* getWayChunk(GeoDataPlacemark *placemark, qint64 matchId);
    void concatFirst(GeoDataPlacemark *placemark, WayChunk *chunk);
    void concatLast(GeoDataPlacemark *placemark, WayChunk *chunk);
    void concatBoth(GeoDataPlacemark *placemark, WayChunk *chunk, WayChunk *otherChunk);
    void addRejectedPlacemarks();
    void addWayChunks();
    void modifyDocument();
};

}

#endif


