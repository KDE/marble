//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014       Calin Cruceru  <crucerucalincristian@gmail.com>
//

#ifndef MERGINGPOLYLINENODESANIMATION_H
#define MERGINGPOLYLINENODESANIMATION_H

#include <QObject>
#include <QTimer>

#include "GeoDataCoordinates.h"


namespace Marble
{

class PolylineAnnotation;
class GeoDataLineString;

class MergingPolylineNodesAnimation : public QObject
{
    Q_OBJECT

public:
    explicit MergingPolylineNodesAnimation( PolylineAnnotation *polyline );
    ~MergingPolylineNodesAnimation();

public Q_SLOTS:
    void startAnimation();

Q_SIGNALS:
    void nodesMoved();
    void animationFinished();

private Q_SLOTS:
    void updateNodes();

private:
    qreal nodesDistance();
    GeoDataCoordinates newCoords();

    QTimer *m_timer;

    const int m_firstNodeIndex;
    const int m_secondNodeIndex;

    GeoDataLineString *m_lineString;

    const GeoDataCoordinates m_firstInitialCoords;
    const GeoDataCoordinates m_secondInitialCoords;
};

} // namespace Marble

#endif // MERGINGPOLYLINENODESANIMATION_H
