// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Calin Cruceru <crucerucalincristian@gmail.com>
//

#ifndef MERGINGPOLYGONNODESANIMATION_H
#define MERGINGPOLYGONNODESANIMATION_H

#include <QObject>
#include <QTimer>

#include "GeoDataCoordinates.h"


namespace Marble {

class AreaAnnotation;
class GeoDataLinearRing;

class MergingPolygonNodesAnimation : public QObject
{
    Q_OBJECT

public:
    explicit MergingPolygonNodesAnimation( AreaAnnotation *polygon );
    ~MergingPolygonNodesAnimation() override;

    enum NodesBoundary {
        InnerBoundary,
        OuterBoundary
    };

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

    const int first_i;
    const int first_j;
    const int second_i;
    const int second_j;

    QTimer         *m_timer;
    NodesBoundary   m_boundary;

    GeoDataLinearRing &outerRing;
    QVector<GeoDataLinearRing> &innerRings;

    GeoDataCoordinates m_firstInitialCoords;
    GeoDataCoordinates m_secondInitialCoords;
};

} // namespace Marble

#endif // MERGINGPOLYGONNODESANIMATION_H
