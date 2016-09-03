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
#include "MarbleMath.h"

namespace Marble {

class NodeReducer : public PlacemarkFilter {
public:
    NodeReducer(GeoDataDocument* document, int zoomLevel);
    void process() override;

private:
    template<class T>
    T* reduce(T* lineString)
    {
        qint64 const prevSize = lineString->size();
        if (prevSize < 2) {
            return new T(*lineString);
        }

        T* reducedLine = new T;
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
        m_count += (prevSize - reducedSize);
        return reducedLine;
        //qDebug()<<"Nodes reduced "<<(prevSize - reducedSize)<<endl;
    }

    static qreal resolutionForLevel(int level);

    qreal m_resolution;
    qint64 m_count;
};

}

#endif
