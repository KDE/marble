// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Calin Cruceru <crucerucalincristian@gmail.com>
//

// Self
#include "MergingPolylineNodesAnimation.h"

// Marble
#include "GeoDataLineString.h"
#include "GeoDataPlacemark.h"
#include "PolylineAnnotation.h"

namespace Marble
{

MergingPolylineNodesAnimation::MergingPolylineNodesAnimation(PolylineAnnotation *polyline)
    : m_timer(new QTimer(this))
    ,

    // To avoid long lines and repeated code
    m_firstNodeIndex(polyline->m_firstMergedNode)
    , m_secondNodeIndex(polyline->m_secondMergedNode)
    , m_lineString(static_cast<GeoDataLineString *>(polyline->placemark()->geometry()))
    , m_firstInitialCoords(m_lineString->at(polyline->m_firstMergedNode))
    , m_secondInitialCoords(m_lineString->at(polyline->m_secondMergedNode))
{
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateNodes()));
}

MergingPolylineNodesAnimation::~MergingPolylineNodesAnimation()
{
    delete m_timer;
}

void MergingPolylineNodesAnimation::startAnimation()
{
    static const int timeOffset = 1;
    m_timer->start(timeOffset);
}

void MergingPolylineNodesAnimation::updateNodes()
{
    static const qreal ratio = 0.05;
    const qreal distanceOffset = m_firstInitialCoords.interpolate(m_secondInitialCoords, ratio).sphericalDistanceTo(m_firstInitialCoords) + 0.001;

    if (nodesDistance() < distanceOffset) {
        m_lineString->at(m_secondNodeIndex) = newCoords();
        m_lineString->remove(m_firstNodeIndex);

        Q_EMIT animationFinished();
    } else {
        m_lineString->at(m_firstNodeIndex) = m_lineString->at(m_firstNodeIndex).interpolate(m_secondInitialCoords, ratio);
        m_lineString->at(m_secondNodeIndex) = m_lineString->at(m_secondNodeIndex).interpolate(m_firstInitialCoords, ratio);
        Q_EMIT nodesMoved();
    }
}

GeoDataCoordinates MergingPolylineNodesAnimation::newCoords()
{
    return m_lineString->at(m_firstNodeIndex).interpolate(m_lineString->at(m_secondNodeIndex), 0.5);
}

qreal MergingPolylineNodesAnimation::nodesDistance()
{
    return m_lineString->at(m_firstNodeIndex).sphericalDistanceTo(m_lineString->at(m_secondNodeIndex));
}

} // namespace Marble

#include "moc_MergingPolylineNodesAnimation.cpp"
