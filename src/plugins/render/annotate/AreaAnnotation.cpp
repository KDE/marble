//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson            <g.real.ate@gmail.com>
// Copyright 2013      Thibaut Gridel           <tgridel@free.fr>
// Copyright 2014      Calin-Cristian Cruceru   <crucerucalincristian@gmail.com
//
#include "AreaAnnotation.h"

#include <qmath.h>

#include "GeoDataPlacemark.h"
#include "GeoDataTypes.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "SceneGraphicsTypes.h"
#include "MarbleMath.h"


namespace Marble
{

AreaAnnotation::AreaAnnotation( GeoDataPlacemark *placemark )
    : SceneGraphicsItem( placemark ),
      m_movedNodeIndex( -1 ),
      m_rightClickedNode( -2 ),
      m_viewport( 0 )
{

}

void AreaAnnotation::paint(GeoPainter *painter, const ViewportParams *viewport )
{
    m_viewport = viewport;
    QList<QRegion> regionList;

    painter->save();
    if ( placemark()->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
        const GeoDataPolygon *polygon = static_cast<const GeoDataPolygon*>( placemark()->geometry() );
        const GeoDataLinearRing &outerRing = polygon->outerBoundary();
        const QVector<GeoDataLinearRing> &innerRings = polygon->innerBoundaries();

        // First paint and add to the region list the nodes which form the outerBoundary, as
        // well as the entire outer polygon.
        for ( int i = 0; i < outerRing.size(); ++i ) {
            QRegion newRegion = painter->regionFromEllipse( outerRing.at(i), 15, 15 );

            if ( !m_selectedNodes.contains( i ) ) {
                painter->setBrush( Oxygen::aluminumGray3);
            } else {
                painter->setBrush( Oxygen::aluminumGray6 );
            }

            painter->drawEllipse( outerRing.at(i) , 15, 15 );
            regionList.append( newRegion );
        }

        // Then paint and add to the region list the nodes which form the innerBoundaries
        int sizeOffset = outerRing.size();
        m_innerBoundariesList.clear();

        foreach ( const GeoDataLinearRing &ring, innerRings ) {
            for ( int i = 0; i < ring.size(); ++i ) {
                QRegion newRegion = painter->regionFromEllipse( ring.at(i), 15, 15 );

                if ( !m_selectedNodes.contains( i + sizeOffset ) ) {
                    painter->setBrush( Oxygen::aluminumGray3 );
                } else {
                    painter->setBrush( Oxygen::aluminumGray6 );
                }

                painter->drawEllipse( ring.at(i), 15, 15 );
                regionList.append( newRegion );
            }
            sizeOffset += ring.size();
            m_innerBoundariesList.append( painter->regionFromPolygon( ring, Qt::OddEvenFill ) );
        }

        regionList.append( painter->regionFromPolygon( outerRing, Qt::OddEvenFill ) );
    }
    painter->restore();
    setRegions( regionList );
}

bool AreaAnnotation::mousePressEvent( QMouseEvent *event )
{
    QList<QRegion> regionList = regions();
    qreal lat, lon;
    m_viewport->geoCoordinates( event->pos().x(), event->pos().y(),
                                lon, lat,
                                GeoDataCoordinates::Radian );



    m_movedPointCoords.set( lon, lat );
    // We loop through all regions from the list, including the last one, which
    // is the entire polygon. This will be useful in mouseMoveEvent to know
    // whether to move a node or the whole polygon.
    for ( int i = 0; i < regionList.size(); ++i ) {
        if ( regionList.at(i).contains( event->pos()) ) {

            // Check if the clicked region is an inner boundary of the polygon.
            if ( i == regionList.size() - 1 && isInnerBoundsPoint( event->pos() ) ) {
                m_rightClickedNode = -2;
                return false;
            }

            if ( event->button() == Qt::LeftButton ) {
                m_movedNodeIndex = i;
                return true;

            } else if ( event->button() == Qt::RightButton ) {
                if ( i < regionList.size() - 1 ) {
                    m_rightClickedNode = i;
                } else {
                    m_rightClickedNode = -1;
                }

                // Return false because we cannot fully deal with this event within this class.
                // We need to have access to the marble widget to show a menu of options on the
                // screen as well as control of the object since one of the options will be
                // "remove polygon".
                return false;
            }
        }
    }

    return false;
}

bool AreaAnnotation::mouseMoveEvent( QMouseEvent *event )
{
    if ( !m_viewport ) {
        return false;
    }

    if ( m_movedNodeIndex < 0 ) {
        return false;
    }

    QList<QRegion> regionList = regions();
    qreal lon, lat;

    m_viewport->geoCoordinates( event->pos().x(),
                                event->pos().y(),
                                lon, lat,
                                GeoDataCoordinates::Radian );

    GeoDataCoordinates const coords( lon, lat );

    // This means one of the nodes has been clicked. The clicked node can be on the outer
    // boundary of the polygon as well as on its inner boundary.
    if ( m_movedNodeIndex >= 0 && m_movedNodeIndex < regionList.size() - 1 ) {
        if ( placemark()->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
            GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark()->geometry() );
            GeoDataLinearRing &outerRing = polygon->outerBoundary();

            // This means the clicked node is one of the nodes which form one of the
            // polygon's inner boundaries.
            if ( m_movedNodeIndex >= outerRing.size() ) {
                int newIndex = m_movedNodeIndex - outerRing.size();
                QVector<GeoDataLinearRing> &innerRings = polygon->innerBoundaries();

                for ( int i = 0; i < innerRings.size(); ++i ) {
                    if ( newIndex - innerRings.at(i).size() < 0 ) {
                        innerRings[i].at(newIndex) = coords;
                        break;
                    } else {
                        newIndex -= innerRings.at(i).size();
                    }
                }
            } else {
                outerRing[m_movedNodeIndex] = coords;
            }
            return true;
        } else {
            return false;
        }
    }

    // This means the interior of the polygon has been clicked (excepting its "holes" - its
    // inner boundaries) and here we handle the move of the entire polygon.
    Q_ASSERT( m_movedNodeIndex == regionList.size() - 1 );
    qreal bearing = m_movedPointCoords.bearing( coords );
    qreal distance = distanceSphere( coords, m_movedPointCoords );

    if ( placemark()->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {

        GeoDataPolygon *poly = static_cast<GeoDataPolygon*>( placemark()->geometry() );
        GeoDataLinearRing outerRing = poly->outerBoundary();
        QVector<GeoDataLinearRing> innerRings = poly->innerBoundaries();

        poly->outerBoundary().clear();
        poly->innerBoundaries().clear();

        for ( int i = 0; i < outerRing.size(); ++i ) {
            GeoDataCoordinates movedPoint = outerRing.at(i).moveByBearing( bearing, distance );
            qreal lon = movedPoint.longitude();
            qreal lat = movedPoint.latitude();

            GeoDataCoordinates::normalizeLonLat( lon, lat );
            movedPoint.setLongitude( lon );
            movedPoint.setLatitude( lat );

            poly->outerBoundary().append( movedPoint );
        }

        foreach ( const GeoDataLinearRing &ring, innerRings ) {
            GeoDataLinearRing newRing( Tessellate );
            for ( int i = 0; i < ring.size(); ++i ) {
                GeoDataCoordinates movedPoint = ring.at(i).moveByBearing( bearing, distance );
                qreal lon = movedPoint.longitude();
                qreal lat = movedPoint.latitude();

                GeoDataCoordinates::normalizeLonLat( lon, lat );
                movedPoint.setLongitude( lon );
                movedPoint.setLatitude( lat );

                newRing.append( movedPoint );
            }
            poly->innerBoundaries().append( newRing );
        }

        m_movedPointCoords.set( lon, lat );
        return true;
    }

    return false;
}

bool AreaAnnotation::mouseReleaseEvent( QMouseEvent *event )
{
    // If the event is caught in one of the polygon's holes, we return false in
    // order to pass it to other potential polygons which have been drawn there.
    if ( isInnerBoundsPoint( event->pos() ) && m_movedNodeIndex == -1 ) {
        return false;
    }

    QList<QRegion> regionList = regions();

    m_movedNodeIndex = -1;
    m_rightClickedNode = -2;

    qreal x, y;
    m_viewport->screenCoordinates( m_movedPointCoords.longitude(), m_movedPointCoords.latitude(), x, y );

    // The node gets selected only if it is clicked and not moved.
    // Is this value ok in order to avoid this?
    if ( qFabs(event->pos().x() - x) > 1 ||
         qFabs(event->pos().y() - y) > 1 ) {
        return true;
    }

    // Only loop until size - 1 because we only want to mark nodes
    // as selected, and not the entire polygon.
    for ( int i = 0; i < regionList.size() - 1; ++i ) {
        if ( regionList.at(i).contains( event->pos()) ) {

            if ( event->button() == Qt::LeftButton ) {
                if ( !m_selectedNodes.contains( i ) ) {
                    m_selectedNodes.append( i );
                } else {
                    m_selectedNodes.removeAll( i );
                }

                return true;
            }
        }
    }

    // We return true even if we get here, because it means that there were no nodes to
    // be marked (the interior of the polygon has been clicked) and we don't want to do
    // anything else than release it, so we tell caller that we handled the event.
    return true;
}

QList<int> &AreaAnnotation::selectedNodes()
{
    return m_selectedNodes;
}

int AreaAnnotation::rightClickedNode() const
{
    return m_rightClickedNode;
}

bool AreaAnnotation::isInnerBoundsPoint( const QPoint &point ) const
{
    foreach ( const QRegion &innerRegion, m_innerBoundariesList ) {
        if ( innerRegion.contains( point ) )
            return true;
    }

    return false;
}

bool AreaAnnotation::isValidPolygon() const
{
    const GeoDataPolygon *poly = static_cast<const GeoDataPolygon*>( placemark()->geometry() );

    foreach ( const GeoDataLinearRing &innerRing, poly->innerBoundaries() ) {
        for ( int i = 0; i < innerRing.size(); ++i ) {
            if ( !poly->outerBoundary().contains( innerRing.at(i) ) ) {
                return false;
            }
        }
    }

    return true;
}

const char *AreaAnnotation::graphicType() const
{
    return SceneGraphicTypes::SceneGraphicAreaAnnotation;
}

}
