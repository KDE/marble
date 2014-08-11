//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014       Calin Cruceru  <crucerucalincristian@gmail.com>
//

// Self
#include "MergingNodesAnimation.h"

// Marble
#include "AreaAnnotation.h"
#include "GeoDataCoordinates.h"
#include "GeoDataPolygon.h"
#include "GeoDataPlacemark.h"
#include "MarbleMath.h"


namespace Marble {


MergingNodesAnimation::MergingNodesAnimation( AreaAnnotation *polygon ) :
    m_targetedArea( polygon ),

    // To avoid long lines and repeated code
    first_i( polygon->m_firstMergedNode.first ),
    first_j( polygon->m_firstMergedNode.second ),
    second_i( polygon->m_secondMergedNode.first ),
    second_j( polygon->m_secondMergedNode.second ),
    m_timer( new QTimer( this ) ),

    // To avoid repeating this code section too often
    outerRing( static_cast<GeoDataPolygon*>( polygon->placemark()->geometry() )->outerBoundary() ),
    innerRings( static_cast<GeoDataPolygon*>( polygon->placemark()->geometry() )->innerBoundaries() )
{
    if ( first_j == -1 ) {
        Q_ASSERT( second_j == -1 );
        m_boundary = OuterBoundary;
        m_firstInitialCoords = outerRing.at( first_i );
        m_secondInitialCoords = outerRing.at( second_i );
    } else {
        Q_ASSERT( first_j != -1 && second_j != -1 );
        m_firstInitialCoords = innerRings.at(first_i).at(first_j);
        m_secondInitialCoords = innerRings.at(second_i).at(second_j);
        m_boundary = InnerBoundary;
    }

    connect( m_timer, SIGNAL(timeout()), this, SLOT(updateNodes()) );
}

MergingNodesAnimation::~MergingNodesAnimation()
{
    delete m_timer;
}

void MergingNodesAnimation::startAnimation()
{
    static const int timeOffset = 1;
    m_timer->start( timeOffset );
}

void MergingNodesAnimation::updateNodes()
{
    static const qreal ratio = 0.05;
    const qreal distanceOffset = distanceSphere( m_firstInitialCoords.interpolate( m_secondInitialCoords, ratio ),
                                                 m_firstInitialCoords ) + 0.001;

    if ( nodesDistance() <  distanceOffset ) {
        if ( m_boundary == OuterBoundary ) {
            outerRing[second_i] = newCoords();
            outerRing.remove( first_i );
        } else {
            innerRings[second_i][second_j] = newCoords();
            innerRings[second_i].remove( first_j );
        }

        emit animationFinished( m_targetedArea );
    } else {
        if ( m_boundary == OuterBoundary ) {
            GeoDataCoordinates first, second;
            first = outerRing.at(first_i).interpolate( m_secondInitialCoords, ratio );
            second = outerRing.at(second_i).interpolate( m_firstInitialCoords, ratio );

            outerRing[first_i] = first;
            outerRing[second_i] = second;
        } else {
            GeoDataCoordinates first, second;
            first = innerRings.at(first_i).at(first_j).interpolate( m_secondInitialCoords, ratio );
            second = innerRings.at(second_i).at(second_j).interpolate( m_firstInitialCoords, ratio );

            innerRings[first_i][first_j] = first;
            innerRings[second_i][second_j] = second;
        }

        emit nodesMoved();
    }
}

GeoDataCoordinates MergingNodesAnimation::newCoords()
{
    return m_boundary == OuterBoundary ?
                outerRing.at(first_i).interpolate( outerRing.at(second_i), 0.5 ) :
                innerRings.at(first_i).at(first_j).interpolate( innerRings.at(second_i).at(second_j), 0.5 );
}

qreal MergingNodesAnimation::nodesDistance()
{
    return m_boundary == OuterBoundary ?
                distanceSphere( outerRing.at(first_i), outerRing.at(second_i) ) :
                distanceSphere( innerRings.at(first_i).at(first_j), innerRings.at(second_i).at(second_j) );
}

} // namespace Marble

#include "MergingNodesAnimation.moc"
