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

#include "GeoDataPlacemark.h"
#include "GeoDataDocument.h"
#include "GeoDataObject.h"
#include "OsmPlacemarkData.h"
#include "StyleBuilder.h"

#include "WayConcatenator.h"
#include "WayChunk.h"
#include "TagsFilter.h"

namespace Marble
{

WayConcatenator::WayConcatenator(GeoDataDocument *document, const QStringList &tagsList, bool andFlag) : TagsFilter(document, tagsList, andFlag)
{
    // qDebug()<< "Entered WayConcatenator";
}

WayConcatenator::~WayConcatenator()
{
    QVector<WayChunk*>::iterator itr = m_chunks.begin();
    for (; itr != m_chunks.end(); ++itr) {
        delete *itr;
    }
}

void WayConcatenator::process()
{
    qint64 count = 0;
    qint64 chunkCount = 0;
    qint64 newCount = 0;
    qint64 placemarkCount = 0;

    // qDebug()<<"** Number of TagFiletered placemarks "<< m_objects.size();
    foreach (GeoDataPlacemark* placemark, placemarks()) {
        qDebug()<<" ";
        ++placemarkCount;
        // qDebug()<<"No."<<plcCount;
        if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType) {
            qDebug()<<"-- Placemark ID : "<<placemark->osmData().id()<<" visualCategory: "<<StyleBuilder::visualCategoryName(placemark->visualCategory());
            GeoDataLineString *line = static_cast<GeoDataLineString*>(placemark->geometry());
            qint64 firstId = placemark->osmData().nodeReference(line->first()).id();
            qint64 lastId = placemark->osmData().nodeReference(line->last()).id();

            bool containsFirst = m_hash.contains(firstId);
            bool containsLast = m_hash.contains(lastId);

            if (!containsFirst && !containsLast) {
                qDebug()<<"No coords matched, creating a new chunk";
                createWayChunk(placemark, firstId, lastId);
                ++count;
                ++chunkCount;
            } else if (containsFirst && !containsLast) {
                qDebug()<<"First coord matched";
                WayChunk *chunk = getWayChunk(placemark, firstId);
                if (chunk != nullptr) {
                    // qDebug()<< "First* Chunk found, concatenating to it";
                    concatFirst(placemark, chunk);
                } else {
                    // qDebug()<<"";
                    qDebug()<< "First* No possible chunk found, creating a new chunk";
                    qDebug()<<"FirstId"<<firstId;
                    qDebug()<<"lastId"<<lastId;
                    // qDebug()<<"";
                    createWayChunk(placemark, firstId, lastId);
                    ++chunkCount;
                    ++newCount;
                }
                ++count;
            } else if (!containsFirst && containsLast) {
                qDebug()<<"Last coord matched";
                WayChunk *chunk = getWayChunk(placemark, lastId);
                if (chunk != nullptr) {
                    // qDebug()<< "Last* Chunk found, concatenating to it";
                    concatLast(placemark, chunk);
                } else {
                    // qDebug()<<"";
                    qDebug()<< "Last* No possible chunk found, creating a new chunk";
                    qDebug()<<"FirstId"<<firstId;
                    qDebug()<<"lastId"<<lastId;
                    // qDebug()<<"";
                    createWayChunk(placemark, firstId, lastId);
                    ++chunkCount;
                    ++newCount;
                }
                ++count;
            } else if (containsFirst && containsLast) {
                qDebug()<<"Both coord matched";
                WayChunk *chunk = getWayChunk(placemark, firstId);
                WayChunk *otherChunk = getWayChunk(placemark, lastId);

                if (chunk != nullptr && otherChunk != nullptr) {
                    // qDebug()<< "Both* Both chunks found, concatenating to it";
                    if(chunk == otherChunk) {
                        qDebug()<<"#### Both the chunks are same, directly adding to the list of placemarks";
                        m_wayPlacemarks.append(*placemark);
                    } else {
                        concatBoth(placemark, chunk, otherChunk);
                        ++count;
                    }
                } else if(chunk != nullptr && otherChunk == nullptr) {
                    // qDebug()<< "Both* First chunk found, concatenating to it";
                    concatFirst(placemark, chunk);
                    ++count;
                } else if(chunk == nullptr && otherChunk != nullptr) {
                    // qDebug()<< "Both* Last chunk found, concatenating to it";
                    concatLast(placemark, otherChunk);
                    ++count;
                } else {
                    // qDebug()<<"";
                    qDebug()<< "Both* No possible chunk found, creating a new chunk";
                    qDebug()<<"FirstId"<<firstId;
                    qDebug()<<"lastId"<<lastId;
                    // qDebug()<<"";
                    createWayChunk(placemark, firstId, lastId);
                    ++chunkCount;
                    ++newCount;
                    ++count;
                }

            }

            // if(flag) {
            // 	qDebug()<<" Concat not possible";
            // 	m_wayPlacemarks.append(*placemark);
            // }
        } else{
            m_wayPlacemarks.append(*placemark);
        }
    }

    addRejectedPlacemarks();
    addWayChunks();
    modifyDocument();

    qDebug()<<"####################################";
    qDebug()<<"Total OSM ways concatenated: "<<count;
    qDebug()<<"* Counted no. of chunks: "<<chunkCount;
    qDebug()<<"* Chunks formed due to no match"<<newCount;
    // qDebug()<<"Total reverses required: "<<rvr;

}

void WayConcatenator::addRejectedPlacemarks()
{
    QVector<GeoDataPlacemark*>::const_iterator itr = rejectedObjectsBegin();
    QVector<GeoDataPlacemark*>::const_iterator endItr = rejectedObjectsEnd();
    for (; itr != endItr; ++itr) {
        m_wayPlacemarks << **itr;
    }
}

void WayConcatenator::addWayChunks()
{
    qint64 totalSize = 0;
    QSet<WayChunk*> chunkSet;
    // QList<WayChunk*> chunkList = m_hash.values();
    // QList<WayChunk*>::iterator cItr = chunkList.begin();
    // qDebug()<<"* Chunk list size = "<<chunkList.size();

    QVector<WayChunk*>::iterator itr = m_chunks.begin();
    for (; itr != m_chunks.end(); ++itr) {
        if (!chunkSet.contains(*itr)) {
            chunkSet.insert(*itr);
            GeoDataPlacemark* placemark = (*itr)->merge();
            if (placemark) {
                m_wayPlacemarks.append(*placemark);
                totalSize += (*itr)->size();
                qDebug()<<"Chunk:";
                (*itr)->printIds();
                qDebug()<<"Size of this chunk"<<(*itr)->size();
                qDebug()<<"Merged";
                qDebug()<<" ";
                delete placemark;
            }
        }
    }
    qDebug()<<"*** Total number of ways merged"<<totalSize;
    qDebug()<<"******* m_chunks vector size"<<m_chunks.size();
    // qDebug()<< "Entered 1";
}

void WayConcatenator::modifyDocument()
{
    document()->clear();
    QVector<GeoDataPlacemark>::iterator itr;
    itr = m_wayPlacemarks.begin();
    for (; itr != m_wayPlacemarks.end(); ++itr) {
        GeoDataPlacemark *placemark = new GeoDataPlacemark(*itr);
        document()->append(placemark);
    }
}

void WayConcatenator::createWayChunk(GeoDataPlacemark *placemark, qint64 firstId, qint64 lastId)
{
    WayChunk *chunk = new WayChunk(placemark, firstId, lastId);
    m_hash.insert(firstId, chunk);
    if (firstId != lastId) {
        m_hash.insert(lastId, chunk);
    }
    m_chunks.append(chunk);
}

WayChunk* WayConcatenator::getWayChunk(GeoDataPlacemark *placemark, qint64 matchId)
{
    qDebug()<<"Searching for a compatible WayChunk";
    qDebug()<<"Visual category for placemark"<<StyleBuilder::visualCategoryName(placemark->visualCategory());

    QHash<qint64, WayChunk*>::iterator matchItr = m_hash.find(matchId);
    while (matchItr != m_hash.end() && matchItr.key() == matchId) {
        WayChunk *chunk = matchItr.value();
        qDebug()<<"		* Chunk ID: "<<chunk->id()<<" Visual category for chunk"<<StyleBuilder::visualCategoryName(chunk->visualCategory());
        if (chunk->concatPossible(placemark)) {
            qDebug()<<"Match found";
            return chunk;
        }
        ++matchItr;
    }
    qDebug()<<"### No Chunk found, returning nullptr";
    return nullptr;
}

void WayConcatenator::concatFirst(GeoDataPlacemark *placemark, WayChunk *chunk)
{
    qDebug()<<"First coord matched";
    qDebug()<<"Matched with: ";
    chunk->printIds();

    GeoDataLineString *line = static_cast<GeoDataLineString*>(placemark->geometry());
    qint64 firstId = placemark->osmData().nodeReference(line->first()).id();
    qint64 lastId = placemark->osmData().nodeReference(line->last()).id();

    if (chunk->first() != chunk->last()) {
        int chunksRemoved = m_hash.remove(firstId, chunk);
        Q_ASSERT(chunksRemoved == 1);
    }
    m_hash.insert(lastId, chunk);

    if (firstId == chunk->last()) {
        //First node matches with an existing last node
        qDebug()<<"Appended chunk";
        chunk->append(placemark, lastId);
    } else {
        //First node matches with an existing first node
        //Reverse the GeoDataLineString of the placemark
        line->reverse();
        chunk->prepend(placemark, lastId);
        qDebug()<<"Reversed line and then prepended";
    }

}

void WayConcatenator::concatLast(GeoDataPlacemark *placemark, WayChunk *chunk)
{
    qDebug()<<"Last coord matched";
    qDebug()<<"Matched with: ";
    chunk->printIds();

    GeoDataLineString *line = static_cast<GeoDataLineString*>(placemark->geometry());
    qint64 firstId = placemark->osmData().nodeReference(line->first()).id();
    qint64 lastId = placemark->osmData().nodeReference(line->last()).id();

    if (chunk->first() != chunk->last()) {
        int chunksRemoved = m_hash.remove(lastId, chunk);
        Q_ASSERT(chunksRemoved == 1);
    }
    m_hash.insert(firstId, chunk);

    if (lastId == chunk->first()) {
        qDebug()<<"Prepended chunk";
        chunk->prepend(placemark, firstId);
    } else {
        line->reverse();
        chunk->append(placemark, firstId);
        qDebug()<<"Reversed line and then appended";
    }

}

void WayConcatenator::concatBoth(GeoDataPlacemark *placemark, WayChunk *chunk, WayChunk *otherChunk)
{

    qDebug()<<" Concat possible";
    qDebug()<<"Inserting in the middle";
    qDebug()<<"Matched first coord with: ";
    chunk->printIds();
    qDebug()<<"Matched last coord with";
    otherChunk->printIds();

    GeoDataLineString *line = static_cast<GeoDataLineString*>(placemark->geometry());
    qint64 firstId = placemark->osmData().nodeReference(line->first()).id();
    qint64 lastId = placemark->osmData().nodeReference(line->last()).id();

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
        qDebug()<<" otherChunk reversed";
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
