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
        QVector<GeoDataCoordinates>::iterator itCoords = lineString->begin();
        GeoDataCoordinates currentCoords = *itCoords;
        reducedLine->append(*itCoords);
        ++itCoords;
        for (; itCoords != (lineString->end() - 1); ++itCoords) {
            if (distanceSphere( currentCoords, *itCoords ) >= m_resolution) {
                currentCoords = *itCoords;
                reducedLine->append(*itCoords);
            }
        }
        reducedLine->append(*itCoords);

        qint64 reducedSize = reducedLine->size();
        m_count += (prevSize - reducedSize);
        return reducedLine;
        //qDebug()<<"Nodes reduced "<<(prevSize - reducedSize)<<endl;
    }

    static qreal resolutionForLevel(int level);

    qreal m_resolution;
    qint64 m_count;
};

#endif
