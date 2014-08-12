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
#include "GeoDataLineString.h"


namespace Marble
{

class PolylineAnnotation;

class MergingPolylineNodesAnimation : public QObject
{
    Q_OBJECT

public:
    explicit MergingPolylineNodesAnimation( PolylineAnnotation *polyline );
    ~MergingPolylineNodesAnimation();

public slots:
    void startAnimation();

signals:
    void nodesMoved();
    void animationFinished( PolylineAnnotation *targeted );

private slots:
    void updateNodes();

private:
    qreal nodesDistance();
    GeoDataCoordinates newCoords();

    PolylineAnnotation *m_targetedPolyline;
    QTimer *m_timer;

    int m_firstNodeIndex;
    int m_secondNodeIndex;

    GeoDataLineString *m_lineString;

    GeoDataCoordinates m_firstInitialCoords;
    GeoDataCoordinates m_secondInitialCoords;
};

} // namespace Marble

#endif // MERGINGPOLYLINENODESANIMATION_H
