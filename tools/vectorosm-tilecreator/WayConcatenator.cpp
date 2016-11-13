//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Akshat Tandon <akshat.tandon@research.iiit.ac.in>
//

#include <QList>
#include <QHash>

#include "GeoDataTypes.h"
#include "GeoDataPlacemark.h"
#include "GeoDataDocument.h"
#include "GeoDataObject.h"
#include "GeoDataLineString.h"
#include "OsmPlacemarkData.h"
#include "StyleBuilder.h"
#include "OsmObjectManager.h"

#include "WayConcatenator.h"
#include "WayChunk.h"
#include "TagsFilter.h"

namespace Marble {

WayConcatenator::WayConcatenator(GeoDataDocument *document) :
    BaseFilter(document),
    m_originalWays(0),
    m_mergedWays(0)
{
    typedef QSharedPointer<GeoDataPlacemark> PlacemarkPtr;
    foreach (GeoDataPlacemark* original, placemarks()) {
        PlacemarkPtr placemark = PlacemarkPtr(new GeoDataPlacemark(*original));
        OsmObjectManager::initializeOsmData(placemark.data());
        bool isWay = false;
        if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType) {
            OsmPlacemarkData const & osmData = placemark->osmData();
            isWay = osmData.containsTagKey("highway") ||
                    osmData.containsTagKey("railway") ||
                    osmData.containsTagKey("waterway");
            if (isWay) {
                GeoDataLineString *line = static_cast<GeoDataLineString*>(placemark->geometry());
                qint64 firstId = osmData.nodeReference(line->first()).oid();
                qint64 lastId = osmData.nodeReference(line->last()).oid();
                if (firstId > 0 && lastId > 0) {
                    ++m_originalWays;
                    bool containsFirst = m_hash.contains(firstId);
                    bool containsLast = m_hash.contains(lastId);

                    if (!containsFirst && !containsLast) {
                        createWayChunk(placemark, firstId, lastId);
                    } else if (containsFirst && !containsLast) {
                        auto chunk = wayChunk(*placemark, firstId);
                        if (chunk != nullptr) {
                            concatFirst(placemark, chunk);
                        } else {
                            createWayChunk(placemark, firstId, lastId);
                        }
                    } else if (!containsFirst && containsLast) {
                        auto chunk = wayChunk(*placemark, lastId);
                        if (chunk != nullptr) {
                            concatLast(placemark, chunk);
                        } else {
                            createWayChunk(placemark, firstId, lastId);
                        }
                    } else if (containsFirst && containsLast) {
                        auto chunk = wayChunk(*placemark, firstId);
                        auto otherChunk = wayChunk(*placemark, lastId);

                        if (chunk != nullptr && otherChunk != nullptr) {
                            if(chunk == otherChunk) {
                                m_wayPlacemarks.append(placemark);
                            } else {
                                concatBoth(placemark, chunk, otherChunk);
                            }
                        } else if(chunk != nullptr && otherChunk == nullptr) {
                            concatFirst(placemark, chunk);
                        } else if(chunk == nullptr && otherChunk != nullptr) {
                            concatLast(placemark, otherChunk);
                        } else {
                            createWayChunk(placemark, firstId, lastId);
                        }
                    }
                } else {
                    isWay = false;
                }
            }
        }

        if (!isWay) {
            m_otherPlacemarks << new GeoDataPlacemark(*original);
        }
    }

    prepareDocument();
    addWayChunks();
}

int WayConcatenator::originalWays() const
{
    return m_originalWays;
}

int WayConcatenator::mergedWays() const
{
    return m_mergedWays;
}

void WayConcatenator::addWayChunks()
{
    for (auto const &placemark: m_wayPlacemarks) {
        document()->append(new GeoDataPlacemark(*placemark));
    }

    QSet<WayChunk::Ptr> chunkSet;
    auto itr = m_chunks.begin();
    for (; itr != m_chunks.end(); ++itr) {
        if (!chunkSet.contains(*itr)) {
            ++m_mergedWays;
            chunkSet.insert(*itr);
            PlacemarkPtr placemark = (*itr)->merge();
            if (placemark) {
                document()->append(new GeoDataPlacemark(*placemark));
            }
        }
    }

    m_chunks.clear();
}

void WayConcatenator::prepareDocument()
{
    document()->clear();
    for (auto placemark: m_otherPlacemarks) {
        document()->append(placemark);
    }
}

void WayConcatenator::createWayChunk(const PlacemarkPtr &placemark, qint64 firstId, qint64 lastId)
{
    WayChunk::Ptr chunk = WayChunk::Ptr(new WayChunk(placemark, firstId, lastId));
    m_hash.insert(firstId, chunk);
    if (firstId != lastId) {
        m_hash.insert(lastId, chunk);
    }
    m_chunks.append(chunk);
}

WayChunk::Ptr WayConcatenator::wayChunk(const GeoDataPlacemark &placemark, qint64 matchId) const
{
    QHash<qint64, WayChunk::Ptr>::ConstIterator matchItr = m_hash.find(matchId);
    while (matchItr != m_hash.end() && matchItr.key() == matchId) {
        auto chunk = matchItr.value();
        if (chunk->concatPossible(placemark)) {
            return chunk;
        }
        ++matchItr;
    }
    return WayChunk::Ptr();
}

void WayConcatenator::concatFirst(const PlacemarkPtr &placemark, const WayChunk::Ptr &chunk)
{
    GeoDataLineString *line = static_cast<GeoDataLineString*>(placemark->geometry());
    qint64 firstId = placemark->osmData().nodeReference(line->first()).oid();
    qint64 lastId = placemark->osmData().nodeReference(line->last()).oid();

    if (chunk->first() != chunk->last()) {
        int chunksRemoved = m_hash.remove(firstId, chunk);
        Q_ASSERT(chunksRemoved == 1);
    }
    m_hash.insert(lastId, chunk);

    if (firstId == chunk->last()) {
        //First node matches with an existing last node
        chunk->append(placemark, lastId);
    } else {
        //First node matches with an existing first node
        //Reverse the GeoDataLineString of the placemark
        line->reverse();
        chunk->prepend(placemark, lastId);
    }
}

void WayConcatenator::concatLast(const PlacemarkPtr &placemark, const WayChunk::Ptr &chunk)
{
    GeoDataLineString *line = static_cast<GeoDataLineString*>(placemark->geometry());
    qint64 firstId = placemark->osmData().nodeReference(line->first()).oid();
    qint64 lastId = placemark->osmData().nodeReference(line->last()).oid();

    if (chunk->first() != chunk->last()) {
        int chunksRemoved = m_hash.remove(lastId, chunk);
        Q_ASSERT(chunksRemoved == 1);
    }
    m_hash.insert(firstId, chunk);

    if (lastId == chunk->first()) {
        chunk->prepend(placemark, firstId);
    } else {
        line->reverse();
        chunk->append(placemark, firstId);
    }
}

void WayConcatenator::concatBoth(const PlacemarkPtr &placemark, const WayChunk::Ptr &chunk, const WayChunk::Ptr &otherChunk)
{
    GeoDataLineString *line = static_cast<GeoDataLineString*>(placemark->geometry());
    qint64 firstId = placemark->osmData().nodeReference(line->first()).oid();
    qint64 lastId = placemark->osmData().nodeReference(line->last()).oid();

    int chunksRemoved;
    if (chunk->first() != chunk->last()) {
        chunksRemoved = m_hash.remove(firstId, chunk);
        Q_ASSERT(chunksRemoved == 1);
    }

    if (firstId == chunk->first()) {
        chunk->reverse();
    }

    chunk->append(placemark, lastId);

    if (lastId == otherChunk->last()) {
        otherChunk->reverse();
    }
    chunk->append(otherChunk);

    chunksRemoved = m_hash.remove(otherChunk->first(), otherChunk);
    Q_ASSERT(chunksRemoved == 1);

    if (otherChunk->first() != otherChunk->last()) {
        chunksRemoved = m_hash.remove(otherChunk->last(), otherChunk);
        Q_ASSERT(chunksRemoved == 1);
    }

    m_hash.insert(otherChunk->last(), chunk);

    m_chunks.removeOne(otherChunk);
}

}
