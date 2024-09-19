// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Akshat Tandon <akshat.tandon@research.iiit.ac.in>
//

#ifndef MARBLE_WAYCONCATENATOR_H
#define MARBLE_WAYCONCATENATOR_H

#include "TagsFilter.h"
#include "WayChunk.h"

namespace Marble
{

class WayConcatenator
{
private:
    using PlacemarkPtr = QSharedPointer<GeoDataPlacemark>;

public:
    explicit WayConcatenator(GeoDataDocument *document);

    int originalWays() const;
    int mergedWays() const;

private:
    void createWayChunk(const PlacemarkPtr &placemark, qint64 firstId, qint64 lastId);
    WayChunk::Ptr wayChunk(const GeoDataPlacemark &placemark, qint64 matchId) const;
    void concatFirst(const PlacemarkPtr &placemark, const WayChunk::Ptr &chunk);
    void concatLast(const PlacemarkPtr &placemark, const WayChunk::Ptr &chunk);
    void concatBoth(const PlacemarkPtr &placemark, const WayChunk::Ptr &chunk, const WayChunk::Ptr &otherChunk);
    void addWayChunks(GeoDataDocument *document);

    QMultiHash<qint64, WayChunk::Ptr> m_hash;
    QVector<WayChunk::Ptr> m_chunks;
    QVector<PlacemarkPtr> m_wayPlacemarks;
    QVector<GeoDataFeature *> m_otherPlacemarks;

    int m_originalWays;
    int m_mergedWays;
};

}

#endif
