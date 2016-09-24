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

#include "BaseFilter.h"
#include "MarbleMath.h"

namespace Marble {

class NodeReducer : public BaseFilter {
public:
    NodeReducer(GeoDataDocument* document, int zoomLevel);

    qint64 removedNodes() const;
    qint64 remainingNodes() const;

private:
    template<class T>
    void reduce(T const * lineString, T* reducedLine)
    {
        qint64 const prevSize = lineString->size();
        if (prevSize < 2) {
            m_remainingNodes += prevSize;
            return;
        }

        auto iter = lineString->begin();
        GeoDataCoordinates currentCoords = *iter;
        reducedLine->append(*iter);
        ++iter;
        for (auto const end = lineString->end() - 1; iter != end; ++iter) {
            if (distanceSphere( currentCoords, *iter ) >= m_resolution) {
                currentCoords = *iter;
                reducedLine->append(*iter);
            }
        }
        reducedLine->append(*iter);

        qint64 reducedSize = reducedLine->size();
        m_removedNodes += (prevSize - reducedSize);
        m_remainingNodes += reducedSize;
        //qDebug()<<"Nodes reduced "<<(prevSize - reducedSize)<<endl;
    }

    static qreal resolutionForLevel(int level);

    qreal m_resolution;
    qint64 m_removedNodes;
    qint64 m_remainingNodes;
};

}

#endif
