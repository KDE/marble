// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Calin Cruceru <crucerucalincristian@gmail.com>
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
    ~MergingPolylineNodesAnimation() override;

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
