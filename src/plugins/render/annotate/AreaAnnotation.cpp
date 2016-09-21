//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson  <g.real.ate@gmail.com>
// Copyright 2013      Thibaut Gridel <tgridel@free.fr>
// Copyright 2014      Calin Cruceru  <crucerucalincristian@gmail.com>
//

// Self
#include "AreaAnnotation.h"

// Qt
#include <qmath.h>
#include <QPair>
#include <QApplication>
#include <QPalette>

// Marble
#include "GeoDataPlacemark.h"
#include "GeoDataTypes.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "SceneGraphicsTypes.h"
#include "MarbleMath.h"
#include "MergingPolygonNodesAnimation.h"
#include "PolylineNode.h"
#include "osm/OsmPlacemarkData.h"


namespace Marble {

const int AreaAnnotation::regularDim = 15;
const int AreaAnnotation::selectedDim = 15;
const int AreaAnnotation::mergedDim = 20;
const int AreaAnnotation::hoveredDim = 20;
const QColor AreaAnnotation::regularColor = Oxygen::aluminumGray3;
const QColor AreaAnnotation::mergedColor = Oxygen::emeraldGreen6;

AreaAnnotation::AreaAnnotation( GeoDataPlacemark *placemark ) :
    SceneGraphicsItem( placemark ),
    m_viewport( 0 ),
    m_regionsInitialized( false ),
    m_busy( false ),
    m_hoveredNode( -1, -1 ),
    m_interactingObj( InteractingNothing ),
    m_virtualHovered( -1, -1 )
{
    setPaintLayers(QStringList() << "AreaAnnotation");
}

AreaAnnotation::~AreaAnnotation()
{
    delete m_animation;
}

void AreaAnnotation::paint(GeoPainter *painter, const ViewportParams *viewport , const QString &layer)
{
    Q_UNUSED(layer);
    m_viewport = viewport;
    Q_ASSERT( placemark()->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType );

    painter->save();
    if ( state() == SceneGraphicsItem::DrawingPolygon || !m_regionsInitialized ) {
        setupRegionsLists( painter );
        m_regionsInitialized = true;
    } else {
        updateRegions( painter );
    }

    if ( hasFocus() ) {
        drawNodes( painter );
    }
    painter->restore();
}

bool AreaAnnotation::containsPoint( const QPoint &point ) const
{
    if ( m_busy ) {
        return false;
    }

    if ( state() == SceneGraphicsItem::Editing ) {
        return ( polygonContains( point ) && innerBoundsContain( point ) == -1 ) ||
               outerNodeContains( point ) != -1 ||
               innerNodeContains( point ) != QPair<int, int>( -1, -1 );

    } else if ( state() == SceneGraphicsItem::AddingPolygonHole ) {
        return polygonContains( point ) && outerNodeContains( point ) == -1 &&
               innerNodeContains( point ) == QPair<int, int>( -1, -1 );

    } else if ( state() == SceneGraphicsItem::MergingNodes ) {
        return outerNodeContains( point ) != -1 ||
               innerNodeContains( point ) != QPair<int, int>( -1, -1 );

    } else if ( state() == SceneGraphicsItem::AddingNodes ) {
        return ( polygonContains( point ) && innerBoundsContain( point ) == -1 ) ||
               virtualNodeContains( point ) != QPair<int, int>( -1, -1 ) ||
               innerNodeContains( point ) != QPair<int, int>( -1, -1 ) ||
               outerNodeContains( point ) != -1;
    }

    return false;
}

void AreaAnnotation::dealWithItemChange( const SceneGraphicsItem *other )
{
    Q_UNUSED( other );

    // So far we only deal with item changes when hovering nodes, so that
    // they do not remain hovered when changing the item we interact with.
    if ( state() == SceneGraphicsItem::Editing ) {
        if ( m_hoveredNode != QPair<int, int>( -1, -1 ) ) {
            const int i = m_hoveredNode.first;
            const int j = m_hoveredNode.second;

            if ( j == -1 ) {
                m_outerNodesList[i].setFlag( PolylineNode::NodeIsEditingHighlighted, false );
            } else {
                m_innerNodesList[i][j].setFlag( PolylineNode::NodeIsEditingHighlighted, false );
            }
        }

        m_hoveredNode = QPair<int, int>( -1, -1 );
    } else if ( state() == SceneGraphicsItem::MergingNodes ) {
        if ( m_hoveredNode != QPair<int, int>( -1, -1 ) ) {
            const int i = m_hoveredNode.first;
            const int j = m_hoveredNode.second;

            if ( j == -1 ) {
                m_outerNodesList[i].setFlag( PolylineNode::NodeIsMergingHighlighted, false );
            } else {
                m_innerNodesList[i][j].setFlag( PolylineNode::NodeIsMergingHighlighted, false );
            }
        }

        m_hoveredNode = QPair<int, int>( -1, -1 );
    } else if ( state() == SceneGraphicsItem::AddingNodes ) {
        m_virtualHovered = QPair<int, int>( -1, -1 );
    }
}

void AreaAnnotation::move( const GeoDataCoordinates &source, const GeoDataCoordinates &destination )
{
    GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark()->geometry() );
    GeoDataLinearRing outerRing = polygon->outerBoundary();
    QVector<GeoDataLinearRing> innerRings = polygon->innerBoundaries();
    OsmPlacemarkData *osmData = 0;
    if ( placemark()->hasOsmData() ) {
        osmData = &placemark()->osmData();
    }

    polygon->outerBoundary().clear();
    polygon->innerBoundaries().clear();

    const qreal deltaLat = destination.latitude() - source.latitude();
    const qreal deltaLon = destination.longitude() - source.longitude();

    Quaternion latRectAxis = Quaternion::fromEuler( 0, destination.longitude(), 0);
    Quaternion latAxis = Quaternion::fromEuler( -deltaLat, 0, 0);
    Quaternion lonAxis = Quaternion::fromEuler(0, deltaLon, 0);
    Quaternion rotAxis = latRectAxis * latAxis * latRectAxis.inverse() * lonAxis;

    qreal lonRotated, latRotated;

    for ( int i = 0; i < outerRing.size(); ++i ) {
        Quaternion qpos = outerRing.at(i).quaternion();
        qpos.rotateAroundAxis(rotAxis);
        qpos.getSpherical( lonRotated, latRotated );
        GeoDataCoordinates movedPoint( lonRotated, latRotated, 0 );
        // Keeping the OsmPlacemarkData synchronized with the geometry
        if ( osmData ) {
            osmData->memberReference( -1 ).changeNodeReference( outerRing.at( i ), movedPoint );
        }
        polygon->outerBoundary().append( movedPoint );
    }

    for ( int i = 0; i < innerRings.size(); ++i ) {
        GeoDataLinearRing newRing( Tessellate );
        for ( int j = 0; j < innerRings.at(i).size(); ++j ) {
            Quaternion qpos = innerRings.at(i).at(j).quaternion();
            qpos.rotateAroundAxis(rotAxis);
            qpos.getSpherical( lonRotated, latRotated );
            GeoDataCoordinates movedPoint( lonRotated, latRotated, 0 );
            if ( osmData ) {
                osmData->memberReference( i ).changeNodeReference( innerRings.at( i ).at( j ), movedPoint );
            }
            newRing.append( movedPoint );
        }
        polygon->innerBoundaries().append( newRing );
    }
}

void AreaAnnotation::setBusy( bool enabled )
{
    m_busy = enabled;

    if ( !enabled && m_animation && state() == SceneGraphicsItem::MergingNodes ) {
        // Update the PolylineNodes lists after the animation has finished its execution.
        const int ff = m_firstMergedNode.first;
        const int fs = m_firstMergedNode.second;
        const int sf = m_secondMergedNode.first;
        const int ss = m_secondMergedNode.second;

        if ( ff != -1 && fs == -1 && sf != -1 && ss == -1 ) {
            m_outerNodesList[sf].setFlag( PolylineNode::NodeIsMergingHighlighted, false );
            m_hoveredNode = QPair<int, int>( -1, -1 );

            // Remove the merging node flag and add the NodeIsSelected flag if either one of the
            // merged nodes had been selected before merging them.
            m_outerNodesList[sf].setFlag( PolylineNode::NodeIsMerged, false );
            if ( m_outerNodesList.at(ff).isSelected() ) {
                m_outerNodesList[sf].setFlag( PolylineNode::NodeIsSelected );
            }
            m_outerNodesList.removeAt( ff );

            m_firstMergedNode = QPair<int, int>( -1, -1 );
            m_secondMergedNode = QPair<int, int>( -1, -1 );
        } else if ( ff != -1 && fs != -1 && sf != -1 && ss != -1 ) {
            m_innerNodesList[sf][ss].setFlag( PolylineNode::NodeIsMergingHighlighted, false );
            m_hoveredNode = QPair<int, int>( -1, -1 );

            m_innerNodesList[sf][ss].setFlag( PolylineNode::NodeIsMerged, false );
            if ( m_innerNodesList.at(ff).at(fs).isSelected() ) {
                m_innerNodesList[sf][ss].setFlag( PolylineNode::NodeIsSelected );
            }
            m_innerNodesList[sf].removeAt( fs );

            m_firstMergedNode = QPair<int, int>( -1, -1 );
            m_secondMergedNode = QPair<int, int>( -1, -1 );
        }

        delete m_animation;
    }
}

bool AreaAnnotation::isBusy() const
{
    return m_busy;
}

void AreaAnnotation::deselectAllNodes()
{
    if ( state() != SceneGraphicsItem::Editing ) {
        return;
    }

    for ( int i = 0 ; i < m_outerNodesList.size(); ++i ) {
        m_outerNodesList[i].setFlag( PolylineNode::NodeIsSelected, false );
    }

    for ( int i = 0; i < m_innerNodesList.size(); ++i ) {
        for ( int j = 0; j < m_innerNodesList.at(i).size(); ++j ) {
            m_innerNodesList[i][j].setFlag( PolylineNode::NodeIsSelected, false );
        }
    }
}

void AreaAnnotation::deleteAllSelectedNodes()
{
    if ( state() != SceneGraphicsItem::Editing ) {
        return;
    }

    GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark()->geometry() );
    GeoDataLinearRing &outerRing = polygon->outerBoundary();
    QVector<GeoDataLinearRing> &innerRings = polygon->innerBoundaries();
    OsmPlacemarkData *osmData = 0;
    OsmPlacemarkData initialOsmData;
    if ( placemark()->hasOsmData() ) {
        osmData = &placemark()->osmData();
        initialOsmData = placemark()->osmData();
    }

    // If it proves inefficient, try something different.
    GeoDataLinearRing initialOuterRing = polygon->outerBoundary();
    QVector<GeoDataLinearRing> initialInnerRings = polygon->innerBoundaries();
    const QVector<PolylineNode> initialOuterNodes = m_outerNodesList;
    const QVector< QVector<PolylineNode> > initialInnerNodes = m_innerNodesList;

    for ( int i = 0; i < outerRing.size(); ++i ) {
        if ( m_outerNodesList.at(i).isSelected() ) {
            if ( m_outerNodesList.size() <= 3 ) {
                setRequest( SceneGraphicsItem::RemovePolygonRequest );
                return;
            }
            if ( osmData ) {
                osmData->memberReference( -1 ).removeNodeReference( initialOuterRing.at( i ) );
            }
            m_outerNodesList.removeAt( i );
            outerRing.remove( i );
            --i;
        }
    }

    for ( int i = 0; i < innerRings.size(); ++i ) {
        for ( int j = 0; j < innerRings.at(i).size(); ++j ) {
            if ( m_innerNodesList.at(i).at(j).isSelected() ) {
                if ( m_innerNodesList.at(i).size() <= 3 ) {

                    if ( osmData ) {
                        osmData->removeMemberReference( i );
                    }
                    innerRings.remove( i );
                    m_innerNodesList.removeAt( i );
                    --i;
                    break;
                }

                if ( osmData ) {
                    osmData->memberReference( i ).removeNodeReference( initialInnerRings.at( i ).at( j ) );
                }
                innerRings[i].remove( j );
                m_innerNodesList[i].removeAt( j );
                --j;
            }
        }
    }

    if ( !isValidPolygon() ) {
        if ( osmData ) {
            placemark()->setOsmData( initialOsmData );
        }
        polygon->outerBoundary() = initialOuterRing;
        polygon->innerBoundaries() = initialInnerRings;
        m_outerNodesList = initialOuterNodes;
        m_innerNodesList = initialInnerNodes;
        setRequest( SceneGraphicsItem::InvalidShapeWarning );
    }
}

void AreaAnnotation::deleteClickedNode()
{
    if ( state() != SceneGraphicsItem::Editing ) {
        return;
    }

    GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark()->geometry() );
    GeoDataLinearRing &outerRing = polygon->outerBoundary();
    QVector<GeoDataLinearRing> &innerRings = polygon->innerBoundaries();
    OsmPlacemarkData *osmData = 0;
    OsmPlacemarkData initialOsmData;
    if ( placemark()->hasOsmData() ) {
        osmData = &placemark()->osmData();
        initialOsmData = placemark()->osmData();
    }

    // If it proves inefficient, try something different.
    GeoDataLinearRing initialOuterRing = polygon->outerBoundary();
    QVector<GeoDataLinearRing> initialInnerRings = polygon->innerBoundaries();
    const QVector<PolylineNode> initialOuterNodes = m_outerNodesList;
    const QVector< QVector<PolylineNode> > initialInnerNodes = m_innerNodesList;

    int i = m_clickedNodeIndexes.first;
    int j = m_clickedNodeIndexes.second;

    m_hoveredNode = QPair<int, int>( -1, -1 );

    if ( i != -1 && j == -1 ) {
        if ( m_outerNodesList.size() <= 3 ) {
            setRequest( SceneGraphicsItem::RemovePolygonRequest );
            return;
        }

        // Keep the OsmPlacemarkData synchronized with the geometrys
        if ( osmData ) {
            osmData->removeNodeReference( outerRing.at( i ) );
        }
        outerRing.remove( i );
        m_outerNodesList.removeAt( i );
    } else if ( i != -1 && j != -1 ) {
        if ( m_innerNodesList.at(i).size() <= 3 ) {
            if ( osmData ) {
                osmData->removeMemberReference( i );
            }
            innerRings.remove( i );
            m_innerNodesList.removeAt( i );
            return;
        }
        if ( osmData ) {
            osmData->memberReference( i ).removeNodeReference( innerRings.at( i ).at( j ) );
        }
        innerRings[i].remove( j );
        m_innerNodesList[i].removeAt( j );
    }

    if ( !isValidPolygon() ) {
        if ( osmData ) {
            placemark()->setOsmData( initialOsmData );
        }
        polygon->outerBoundary() = initialOuterRing;
        polygon->innerBoundaries() = initialInnerRings;
        m_outerNodesList = initialOuterNodes;
        m_innerNodesList = initialInnerNodes;
        setRequest( SceneGraphicsItem::InvalidShapeWarning );
    }
}

void AreaAnnotation::changeClickedNodeSelection()
{
    if ( state() != SceneGraphicsItem::Editing ) {
        return;
    }

    const int i = m_clickedNodeIndexes.first;
    const int j = m_clickedNodeIndexes.second;

    if ( i != -1 && j == -1 ) {
        m_outerNodesList[i].setFlag( PolylineNode::NodeIsSelected,
                                     !m_outerNodesList.at(i).isSelected() );
    } else if ( i != -1 && j != -1 ) {
        m_innerNodesList[i][j].setFlag( PolylineNode::NodeIsSelected,
                                        !m_innerNodesList.at(i).at(j).isSelected() );
    }
}

bool AreaAnnotation::hasNodesSelected() const
{
    for ( int i = 0; i < m_outerNodesList.size(); ++i ) {
        if ( m_outerNodesList.at(i).isSelected() ) {
            return true;
        }
    }

    for ( int i = 0; i < m_innerNodesList.size(); ++i ) {
        for ( int j = 0; j < m_innerNodesList.at(i).size(); ++j ) {
            if ( m_innerNodesList.at(i).at(j).isSelected() ) {
                return true;
            }
        }
    }

    return false;
}

bool AreaAnnotation::clickedNodeIsSelected() const
{
    const int i = m_clickedNodeIndexes.first;
    const int j = m_clickedNodeIndexes.second;

    return ( i != -1 && j == -1 && m_outerNodesList.at(i).isSelected() ) ||
           ( i != -1 && j != -1 && m_innerNodesList.at(i).at(j).isSelected() );
}

QPointer<MergingPolygonNodesAnimation> AreaAnnotation::animation()
{
    return m_animation;
}

bool AreaAnnotation::mousePressEvent( QMouseEvent *event )
{
    if ( !m_viewport || m_busy ) {
        return false;
    }

    setRequest( SceneGraphicsItem::NoRequest );

    if ( state() == SceneGraphicsItem::Editing ) {
        return processEditingOnPress( event );
    } else if ( state() == SceneGraphicsItem::AddingPolygonHole ) {
        return processAddingHoleOnPress( event );
    } else if ( state() == SceneGraphicsItem::MergingNodes ) {
        return processMergingOnPress( event );
    } else if ( state() == SceneGraphicsItem::AddingNodes ) {
        return processAddingNodesOnPress( event );
    }

    return false;
}

bool AreaAnnotation::mouseMoveEvent( QMouseEvent *event )
{
    if ( !m_viewport || m_busy ) {
        return false;
    }

    setRequest( SceneGraphicsItem::NoRequest );

    if ( state() == SceneGraphicsItem::Editing ) {
        return processEditingOnMove( event );
    } else if ( state() == SceneGraphicsItem::AddingPolygonHole ) {
        return processAddingHoleOnMove( event );
    } else if ( state() == SceneGraphicsItem::MergingNodes ) {
        return processMergingOnMove( event );
    } else if ( state() == SceneGraphicsItem::AddingNodes ) {
        return processAddingNodesOnMove( event );
    }

    return false;
}

bool AreaAnnotation::mouseReleaseEvent( QMouseEvent *event )
{
    if ( !m_viewport || m_busy ) {
        return false;
    }

    setRequest( SceneGraphicsItem::NoRequest );

    if ( state() == SceneGraphicsItem::Editing ) {
        return processEditingOnRelease( event );
    } else if ( state() == SceneGraphicsItem::AddingPolygonHole ) {
        return processAddingHoleOnRelease( event );
    } else if ( state() == SceneGraphicsItem::MergingNodes ) {
        return processMergingOnRelease( event );
    } else if ( state() == SceneGraphicsItem::AddingNodes ) {
        return processAddingNodesOnRelease( event );
    }

    return false;
}

void AreaAnnotation::dealWithStateChange( SceneGraphicsItem::ActionState previousState )
{
    // Dealing with cases when exiting a state has an effect on this item.
    if ( previousState == SceneGraphicsItem::Editing ) {
        // Make sure that when changing the state, there is no highlighted node.
        if ( m_hoveredNode != QPair<int, int>( -1, -1 ) ) {
            const int i = m_hoveredNode.first;
            const int j = m_hoveredNode.second;

            if ( j == -1 ) {
                m_outerNodesList[i].setFlag( PolylineNode::NodeIsEditingHighlighted, false );
            } else {
                m_innerNodesList[i][j].setFlag( PolylineNode::NodeIsEditingHighlighted, false );
            }
        }

        m_clickedNodeIndexes = QPair<int, int>( -1, -1 );
        m_hoveredNode = QPair<int, int>( -1, -1 );
    } else if ( previousState == SceneGraphicsItem::AddingPolygonHole ) {
        // Check if a polygon hole was being drawn before changing state.
        GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark()->geometry() );
        QVector<GeoDataLinearRing> &innerBounds = polygon->innerBoundaries();

        if ( innerBounds.size() && innerBounds.last().size() <= 2 ) {
            // If only two nodes were added, remove this inner boundary entirely.
            innerBounds.remove( innerBounds.size() - 1 );
            m_innerNodesList.removeLast();
            return;
        }
    } else if ( previousState == SceneGraphicsItem::MergingNodes ) {
        // If there was only a node selected for being merged and the state changed,
        // deselect it.
        const int i = m_firstMergedNode.first;
        const int j = m_firstMergedNode.second;

        if ( i != -1 && j != -1 ) {
            m_innerNodesList[i][j].setFlag( PolylineNode::NodeIsMerged, false );
        } else if ( i != -1 && j == -1 ) {
            m_outerNodesList[i].setFlag( PolylineNode::NodeIsMerged, false );
        }

        // Make sure that when changing the state, there is no highlighted node.
        if ( m_hoveredNode != QPair<int, int>( -1, -1 ) ) {
            int i = m_hoveredNode.first;
            int j = m_hoveredNode.second;

            if ( j == -1 ) {
                m_outerNodesList[i].setFlag( PolylineNode::NodeIsMergingHighlighted, false );
            } else {
                m_innerNodesList[i][j].setFlag( PolylineNode::NodeIsMergingHighlighted, false );
            }
        }

        m_firstMergedNode = QPair<int, int>( -1, -1 );
        m_hoveredNode = QPair<int, int>( -1, -1 );
        delete m_animation;
    } else if ( previousState == SceneGraphicsItem::AddingNodes ) {
        m_outerVirtualNodes.clear();
        m_innerVirtualNodes.clear();
        m_virtualHovered = QPair<int, int>( -1, -1 );
        m_adjustedNode = -2;
    }

    // Dealing with cases when entering a state has an effect on this item, or
    // initializations are needed.
    if ( state() == SceneGraphicsItem::Editing ) {
        m_interactingObj = InteractingNothing;
        m_clickedNodeIndexes = QPair<int, int>( -1, -1 );
        m_hoveredNode = QPair<int, int>( -1, -1 );
    } else if ( state() == SceneGraphicsItem::AddingPolygonHole ) {
        // Nothing to do so far when entering this state.
        GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark()->geometry() );
        QVector<GeoDataLinearRing> &innerBounds = polygon->innerBoundaries();

        m_innerNodesList.append(QVector<PolylineNode>());
        innerBounds.append( GeoDataLinearRing( Tessellate ) );
    } else if ( state() == SceneGraphicsItem::MergingNodes ) {
        m_firstMergedNode = QPair<int, int>( -1, -1 );
        m_secondMergedNode = QPair<int, int>( -1, -1 );
        m_hoveredNode = QPair<int, int>( -1, -1 );
        m_animation = 0;
    } else if ( state() == SceneGraphicsItem::AddingNodes ) {
        m_virtualHovered = QPair<int, int>( -1, -1 );
        m_adjustedNode = -2;
    }
}

const char *AreaAnnotation::graphicType() const
{
    return SceneGraphicsTypes::SceneGraphicAreaAnnotation;
}

bool AreaAnnotation::isValidPolygon() const
{
    const GeoDataPolygon *poly = static_cast<const GeoDataPolygon*>( placemark()->geometry() );
    const QVector<GeoDataLinearRing> &innerRings = poly->innerBoundaries();

    foreach ( const GeoDataLinearRing &innerRing, innerRings ) {
        for ( int i = 0; i < innerRing.size(); ++i ) {
            if ( !poly->outerBoundary().contains( innerRing.at(i) ) ) {
                return false;
            }
        }
    }

    return true;
}

void AreaAnnotation::setupRegionsLists( GeoPainter *painter )
{
    const GeoDataPolygon *polygon = static_cast<const GeoDataPolygon*>( placemark()->geometry() );
    const GeoDataLinearRing &outerRing = polygon->outerBoundary();
    const QVector<GeoDataLinearRing> &innerRings = polygon->innerBoundaries();

    // Add the outer boundary nodes.
    QVector<GeoDataCoordinates>::ConstIterator itBegin = outerRing.constBegin();
    QVector<GeoDataCoordinates>::ConstIterator itEnd = outerRing.constEnd();

    m_outerNodesList.clear();
    m_innerNodesList.clear();
    m_boundariesList.clear();

    for ( ; itBegin != itEnd; ++itBegin ) {
        const PolylineNode newNode = PolylineNode( painter->regionFromEllipse( *itBegin, regularDim, regularDim ) );
        m_outerNodesList.append( newNode );
    }

    foreach ( const GeoDataLinearRing &innerRing, innerRings ) {
        QVector<GeoDataCoordinates>::ConstIterator itBegin = innerRing.constBegin();
        QVector<GeoDataCoordinates>::ConstIterator itEnd = innerRing.constEnd();
        QVector<PolylineNode> innerNodes;
        innerNodes.reserve(innerRing.size());

        for ( ; itBegin != itEnd; ++itBegin ) {
            const PolylineNode newNode = PolylineNode( painter->regionFromEllipse( *itBegin, regularDim, regularDim ) );
            innerNodes.append( newNode );
        }
        m_innerNodesList.append( innerNodes );
    }

    // Add the outer boundary to the boundaries list.
    m_boundariesList.append( painter->regionFromPolygon( outerRing, Qt::OddEvenFill ) );
}

void AreaAnnotation::updateRegions( GeoPainter *painter )
{
    if ( m_busy ) {
        return;
    }

    const GeoDataPolygon *polygon = static_cast<const GeoDataPolygon*>( placemark()->geometry() );
    const GeoDataLinearRing &outerRing = polygon->outerBoundary();
    const QVector<GeoDataLinearRing> &innerRings = polygon->innerBoundaries();


    if ( state() == SceneGraphicsItem::AddingNodes ) {
        // Create and update virtual nodes lists when being in the AddingPolgonNodes state, to
        // avoid overhead in other states.
        m_outerVirtualNodes.clear();
        const QRegion firstRegion( painter->regionFromEllipse( outerRing.first().interpolate(
                                             outerRing.last(), 0.5 ), hoveredDim, hoveredDim ) );
        m_outerVirtualNodes.append( PolylineNode( firstRegion ) );
        for ( int i = 0; i < outerRing.size() - 1; ++i ) {
            const QRegion newRegion( painter->regionFromEllipse( outerRing.at(i).interpolate(
                                             outerRing.at(i+1), 0.5 ), hoveredDim, hoveredDim ) );
            m_outerVirtualNodes.append( PolylineNode( newRegion ) );
        }

        m_innerVirtualNodes.clear();
        m_innerVirtualNodes.reserve(innerRings.size());
        for ( int i = 0; i < innerRings.size(); ++i ) {
            m_innerVirtualNodes.append(QVector<PolylineNode>());
            const QRegion firstRegion( painter->regionFromEllipse( innerRings.at(i).first().interpolate(
                                             innerRings.at(i).last(), 0.5 ), hoveredDim, hoveredDim ) );
            m_innerVirtualNodes[i].append( PolylineNode( firstRegion ) );
            for ( int j = 0; j < innerRings.at(i).size() - 1; ++j ) {
                const QRegion newRegion( painter->regionFromEllipse( innerRings.at(i).at(j).interpolate(
                                             innerRings.at(i).at(j+1), 0.5 ), hoveredDim, hoveredDim ) );
                m_innerVirtualNodes[i].append( PolylineNode( newRegion ) );
            }
        }
    }

    // Update the boundaries list.
    m_boundariesList.clear();
    m_boundariesList.reserve(1 + innerRings.size());

    m_boundariesList.append( painter->regionFromPolygon( outerRing, Qt::OddEvenFill ) );
    foreach ( const GeoDataLinearRing &ring, innerRings ) {
        m_boundariesList.append( painter->regionFromPolygon( ring, Qt::OddEvenFill ) );
    }

    // Update the outer and inner nodes lists.
    Q_ASSERT( m_outerNodesList.size() == outerRing.size() );
    for ( int i = 0; i < m_outerNodesList.size(); ++i ) {
        const QRegion newRegion = m_outerNodesList.at(i).isSelected() ?
                                  painter->regionFromEllipse( outerRing.at(i), selectedDim, selectedDim ) :
                                  painter->regionFromEllipse( outerRing.at(i), regularDim, regularDim );
        m_outerNodesList[i].setRegion( newRegion );
    }

    Q_ASSERT( m_innerNodesList.size() == innerRings.size() );
    for ( int i = 0; i < m_innerNodesList.size(); ++i ) {
        Q_ASSERT( m_innerNodesList.at(i).size() == innerRings.at(i).size() );
        for ( int j = 0; j < m_innerNodesList.at(i).size(); ++j ) {
            const QRegion newRegion = m_innerNodesList.at(i).at(j).isSelected() ?
                           painter->regionFromEllipse( innerRings.at(i).at(j), selectedDim, selectedDim ) :
                           painter->regionFromEllipse( innerRings.at(i).at(j), regularDim, regularDim );
            m_innerNodesList[i][j].setRegion( newRegion );
        }
    }
}

void AreaAnnotation::drawNodes( GeoPainter *painter )
{
    // These are the 'real' dimensions of the drawn nodes. The ones which have class scope are used
    // to generate the regions and they are a little bit larger, because, for example, it would be
    // a little bit too hard to select nodes.
    static const int d_regularDim = 10;
    static const int d_selectedDim = 10;
    static const int d_mergedDim = 20;
    static const int d_hoveredDim = 20;

    const GeoDataPolygon *polygon = static_cast<const GeoDataPolygon*>( placemark()->geometry() );
    const GeoDataLinearRing &outerRing = polygon->outerBoundary();
    const QVector<GeoDataLinearRing> &innerRings = polygon->innerBoundaries();

    QColor glowColor = QApplication::palette().highlightedText().color();
    glowColor.setAlpha(120);

    auto const selectedColor = QApplication::palette().highlight().color();
    auto const hoveredColor = selectedColor;
    for ( int i = 0; i < outerRing.size(); ++i ) {
        // The order here is important, because a merged node can be at the same time selected.
        if ( m_outerNodesList.at(i).isBeingMerged() ) {
            painter->setBrush( mergedColor );
            painter->drawEllipse( outerRing.at(i), d_mergedDim, d_mergedDim );
        } else if ( m_outerNodesList.at(i).isSelected() ) {
            painter->setBrush( selectedColor );
            painter->drawEllipse( outerRing.at(i), d_selectedDim, d_selectedDim );

            if ( m_outerNodesList.at(i).isEditingHighlighted() ||
                 m_outerNodesList.at(i).isMergingHighlighted() ) {
                QPen defaultPen = painter->pen();
                QPen newPen;
                newPen.setWidth( defaultPen.width() + 3 );
                newPen.setColor( glowColor );

                painter->setBrush( Qt::NoBrush );
                painter->setPen( newPen );
                painter->drawEllipse( outerRing.at(i), d_selectedDim + 2, d_selectedDim + 2 );
                painter->setPen( defaultPen );
            }
        } else {
            painter->setBrush( regularColor );
            painter->drawEllipse( outerRing.at(i), d_regularDim, d_regularDim );

            if ( m_outerNodesList.at(i).isEditingHighlighted() ||
                 m_outerNodesList.at(i).isMergingHighlighted() ) {
                QPen defaultPen = painter->pen();
                QPen newPen;
                newPen.setWidth( defaultPen.width() + 3 );
                newPen.setColor( glowColor );

                painter->setPen( newPen );
                painter->setBrush( Qt::NoBrush );
                painter->drawEllipse( outerRing.at(i), d_regularDim + 2, d_regularDim + 2 );
                painter->setPen( defaultPen );
            }
        }
    }

    for ( int i = 0; i < innerRings.size(); ++i ) {
        for ( int j = 0; j < innerRings.at(i).size(); ++j ) {
            if ( m_innerNodesList.at(i).at(j).isBeingMerged() ) {
                painter->setBrush( mergedColor );
                painter->drawEllipse( innerRings.at(i).at(j), d_mergedDim, d_mergedDim );
            } else if ( m_innerNodesList.at(i).at(j).isSelected() ) {
                painter->setBrush( selectedColor );
                painter->drawEllipse( innerRings.at(i).at(j), d_selectedDim, d_selectedDim );

                if ( m_innerNodesList.at(i).at(j).isEditingHighlighted() ||
                     m_innerNodesList.at(i).at(j).isMergingHighlighted() ) {
                    QPen defaultPen = painter->pen();
                    QPen newPen;
                    newPen.setWidth( defaultPen.width() + 3 );
                    newPen.setColor( glowColor );

                    painter->setBrush( Qt::NoBrush );
                    painter->setPen( newPen );
                    painter->drawEllipse( innerRings.at(i).at(j), d_selectedDim + 2, d_selectedDim + 2 );
                    painter->setPen( defaultPen );
                }
            } else {
                painter->setBrush( regularColor );
                painter->drawEllipse( innerRings.at(i).at(j), d_regularDim, d_regularDim );

                if ( m_innerNodesList.at(i).at(j).isEditingHighlighted() ||
                     m_innerNodesList.at(i).at(j).isMergingHighlighted() ) {
                    QPen defaultPen = painter->pen();
                    QPen newPen;
                    newPen.setWidth( defaultPen.width() + 3 );
                    newPen.setColor( glowColor );

                    painter->setBrush( Qt::NoBrush );
                    painter->setPen( newPen );
                    painter->drawEllipse( innerRings.at(i).at(j), d_regularDim + 2, d_regularDim + 2 );
                    painter->setPen( defaultPen );
                }
            }
        }
    }

    if ( m_virtualHovered != QPair<int, int>( -1, -1 ) ) {
        const int i = m_virtualHovered.first;
        const int j = m_virtualHovered.second;

        painter->setBrush( hoveredColor );

        if ( i != -1 && j == -1 ) {
            const GeoDataCoordinates coords = i ?
                                              outerRing.at(i).interpolate( outerRing.at(i - 1), 0.5 ) :
                                              outerRing.first().interpolate( outerRing.last(), 0.5 );
            painter->drawEllipse( coords, d_hoveredDim, d_hoveredDim );
        } else {
            Q_ASSERT( i != -1 && j != -1 );

            const GeoDataCoordinates coords = j ?
                                innerRings.at(i).at(j).interpolate( innerRings.at(i).at(j - 1), 0.5 ) :
                                innerRings.at(i).first().interpolate( innerRings.at(i).last(), 0.5 );
            painter->drawEllipse( coords, d_hoveredDim, d_hoveredDim );
        }
    }
}

int AreaAnnotation::outerNodeContains( const QPoint &point ) const
{
    if ( !hasFocus() ) {
        return -1;
    }

    for ( int i = 0; i < m_outerNodesList.size(); ++i ) {
        if ( m_outerNodesList.at(i).containsPoint( point ) ) {
            return i;
        }
    }

    return -1;
}

QPair<int, int> AreaAnnotation::innerNodeContains( const QPoint &point ) const
{
    if ( !hasFocus() ) {
        return QPair<int, int>( -1, -1 );
    }

    for ( int i = 0; i < m_innerNodesList.size(); ++i ) {
        for ( int j = 0; j < m_innerNodesList.at(i).size(); ++j ) {
            if ( m_innerNodesList.at(i).at(j).containsPoint( point ) ) {
                return QPair<int, int>( i, j );
            }
        }
    }

    return QPair<int, int>( -1, -1 );
}

QPair<int, int> AreaAnnotation::virtualNodeContains( const QPoint &point ) const
{
    if ( !hasFocus() ) {
        return QPair<int, int>( -1, -1 );
    }

    for ( int i = 0; i < m_outerVirtualNodes.size(); ++i ) {
        if ( m_outerVirtualNodes.at(i).containsPoint( point ) ) {
            return QPair<int, int>( i, -1 );
        }
    }

    for ( int i = 0; i < m_innerVirtualNodes.size(); ++i ) {
        for ( int j = 0; j < m_innerVirtualNodes.at(i).size(); ++j ) {
            if ( m_innerVirtualNodes.at(i).at(j).containsPoint( point ) ) {
                return QPair<int, int>( i, j );
            }
        }
    }

    return QPair<int, int>( -1, -1 );
}

int AreaAnnotation::innerBoundsContain( const QPoint &point ) const
{
    // There are no inner boundaries.
    if ( m_boundariesList.size() == 1 ) {
        return -1;
    }

    // Starting from 1 because on index 0 is stored the region representing the whole polygon.
    for ( int i = 1; i < m_boundariesList.size(); ++i ) {
        if ( m_boundariesList.at(i).contains( point ) ) {
            return i;
        }
    }

    return -1;
}

bool AreaAnnotation::polygonContains( const QPoint &point ) const
{
    return m_boundariesList.first().contains( point );
}

bool AreaAnnotation::processEditingOnPress( QMouseEvent *mouseEvent )
{
    if ( mouseEvent->button() != Qt::LeftButton && mouseEvent->button() != Qt::RightButton ) {
        return false;
    }

    qreal lat, lon;
    m_viewport->geoCoordinates( mouseEvent->pos().x(),
                                mouseEvent->pos().y(),
                                lon, lat,
                                GeoDataCoordinates::Radian );
    m_movedPointCoords.set( lon, lat );

    // First check if one of the nodes from outer boundary has been clicked.
    const int outerIndex = outerNodeContains( mouseEvent->pos() );
    if ( outerIndex != -1 ) {
        m_clickedNodeIndexes = QPair<int, int>( outerIndex, -1 );

        if ( mouseEvent->button() == Qt::RightButton ) {
            setRequest( SceneGraphicsItem::ShowNodeRmbMenu );
        } else {
            m_interactingObj = InteractingNode;
        }

        return true;
    }

    // Then check if one of the nodes which form an inner boundary has been clicked.
    const QPair<int, int> innerIndexes = innerNodeContains( mouseEvent->pos() );
    if ( innerIndexes.first != -1 && innerIndexes.second != -1 ) {
        m_clickedNodeIndexes = innerIndexes;

        if ( mouseEvent->button() == Qt::RightButton ) {
            setRequest( SceneGraphicsItem::ShowNodeRmbMenu );
        } else {
            m_interactingObj = InteractingNode;
        }
        return true;
    }

    // If neither outer boundary nodes nor inner boundary nodes contain the event position,
    // then check if the interior of the polygon (excepting its 'holes') contains this point.
    if ( polygonContains( mouseEvent->pos() ) &&
         innerBoundsContain( mouseEvent->pos() ) == -1 ) {
        if ( mouseEvent->button() == Qt::RightButton ) {
            setRequest( SceneGraphicsItem::ShowPolygonRmbMenu );
        } else {
            m_interactingObj = InteractingPolygon;
        }
        return true;
    }

    return false;
}

bool AreaAnnotation::processEditingOnMove( QMouseEvent *mouseEvent )
{
   if ( !m_viewport ) {
        return false;
    }

    qreal lon, lat;
    m_viewport->geoCoordinates( mouseEvent->pos().x(),
                                mouseEvent->pos().y(),
                                lon, lat,
                                GeoDataCoordinates::Radian );
    const GeoDataCoordinates newCoords( lon, lat );

    const qreal deltaLat = lat - m_movedPointCoords.latitude();
    const qreal deltaLon = lon - m_movedPointCoords.longitude();

    if ( m_interactingObj == InteractingNode ) {
        GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark()->geometry() );
        GeoDataLinearRing &outerRing = polygon->outerBoundary();
        QVector<GeoDataLinearRing> &innerRings = polygon->innerBoundaries();
        OsmPlacemarkData *osmData = 0;
        if ( placemark()->hasOsmData() ) {
            osmData = &placemark()->osmData();
        }

        const int i = m_clickedNodeIndexes.first;
        const int j = m_clickedNodeIndexes.second;

        if ( j == -1 ) {
            // Keeping the osmPlacemarkData synchronized with the geometry
            if ( osmData ) {
                 osmData->memberReference( -1 ).changeNodeReference( outerRing.at( i ), newCoords );
            }
            outerRing[i] = newCoords;
        } else {
            Q_ASSERT( i != -1 && j != -1 );
            if ( osmData ) {
                 osmData->memberReference( i ).changeNodeReference( innerRings.at( i ).at( j ), newCoords );
            }
            innerRings[i].at(j) = newCoords;
        }

        return true;
    } else if ( m_interactingObj == InteractingPolygon ) {
        GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark()->geometry() );
        GeoDataLinearRing outerRing = polygon->outerBoundary();
        QVector<GeoDataLinearRing> innerRings = polygon->innerBoundaries();
        OsmPlacemarkData *osmData = 0;
        if ( placemark()->hasOsmData() ) {
            osmData = &placemark()->osmData();
        }

        Quaternion latRectAxis = Quaternion::fromEuler( 0, lon, 0);
        Quaternion latAxis = Quaternion::fromEuler( -deltaLat, 0, 0);
        Quaternion lonAxis = Quaternion::fromEuler(0, deltaLon, 0);
        Quaternion rotAxis = latRectAxis * latAxis * latRectAxis.inverse() * lonAxis;


        polygon->outerBoundary().clear();
        polygon->innerBoundaries().clear();

        qreal lonRotated, latRotated;

        for ( int i = 0; i < outerRing.size(); ++i ) {
            Quaternion qpos = outerRing.at(i).quaternion();
            qpos.rotateAroundAxis(rotAxis);
            qpos.getSpherical( lonRotated, latRotated );
            GeoDataCoordinates movedPoint( lonRotated, latRotated, 0 );
            if ( osmData ) {
                osmData->memberReference( -1 ).changeNodeReference( outerRing.at( i ), movedPoint );
            }
            polygon->outerBoundary().append( movedPoint );
        }

        for ( int i = 0; i < innerRings.size(); ++i ) {
            GeoDataLinearRing newRing( Tessellate );
            for ( int j = 0; j < innerRings.at(i).size(); ++j ) {
                Quaternion qpos = innerRings.at(i).at(j).quaternion();
                qpos.rotateAroundAxis(rotAxis);
                qpos.getSpherical( lonRotated, latRotated );
                GeoDataCoordinates movedPoint( lonRotated, latRotated, 0 );
                if ( osmData ) {
                    osmData->memberReference( i ).changeNodeReference( innerRings.at( i ).at( j ) , movedPoint );
                }
                newRing.append( movedPoint );
            }
            polygon->innerBoundaries().append( newRing );
        }

        m_movedPointCoords = newCoords;
        return true;
    } else if ( m_interactingObj == InteractingNothing ) {
        return dealWithHovering( mouseEvent );
    }

    return false;
}

bool AreaAnnotation::processEditingOnRelease( QMouseEvent *mouseEvent )
{
    static const int mouseMoveOffset = 1;

    if ( mouseEvent->button() != Qt::LeftButton ) {
        return false;
    }

    if ( m_interactingObj == InteractingNode ) {
        qreal x, y;
        m_viewport->screenCoordinates( m_movedPointCoords.longitude(),
                                       m_movedPointCoords.latitude(),
                                       x, y );
        // The node gets selected only if it is clicked and not moved.
        if ( qFabs(mouseEvent->pos().x() - x) > mouseMoveOffset ||
             qFabs(mouseEvent->pos().y() - y) > mouseMoveOffset ) {
            m_interactingObj = InteractingNothing;
            return true;
        }

        const int i = m_clickedNodeIndexes.first;
        const int j = m_clickedNodeIndexes.second;

        if ( j == -1 ) {
            m_outerNodesList[i].setFlag( PolylineNode::NodeIsSelected,
                                         !m_outerNodesList[i].isSelected() );
        } else {
            m_innerNodesList[i][j].setFlag ( PolylineNode::NodeIsSelected,
                                             !m_innerNodesList.at(i).at(j).isSelected() );
        }

        m_interactingObj = InteractingNothing;
        return true;
    } else if ( m_interactingObj == InteractingPolygon ) {
        // Nothing special happens at polygon release.
        m_interactingObj = InteractingNothing;
        return true;
    }

    return false;
}

bool AreaAnnotation::processAddingHoleOnPress( QMouseEvent *mouseEvent )
{
    if ( mouseEvent->button() != Qt::LeftButton ) {
        return false;
    }

    qreal lon, lat;
    m_viewport->geoCoordinates( mouseEvent->pos().x(),
                                    mouseEvent->pos().y(),
                                    lon, lat,
                                    GeoDataCoordinates::Radian );
    const GeoDataCoordinates newCoords( lon, lat );

    GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark()->geometry() );
    QVector<GeoDataLinearRing> &innerBounds = polygon->innerBoundaries();

    innerBounds.last().append( newCoords );
    m_innerNodesList.last().append( PolylineNode() );

    return true;
}

bool AreaAnnotation::processAddingHoleOnMove( QMouseEvent *mouseEvent )
{
    Q_UNUSED( mouseEvent );
    return true;
}

bool AreaAnnotation::processAddingHoleOnRelease( QMouseEvent *mouseEvent )
{
    Q_UNUSED( mouseEvent );
    return true;
}

bool AreaAnnotation::processMergingOnPress( QMouseEvent *mouseEvent )
{
    if ( mouseEvent->button() != Qt::LeftButton ) {
        return false;
    }

    GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark()->geometry() );
    GeoDataLinearRing initialOuterRing = polygon->outerBoundary();
    OsmPlacemarkData *osmData = 0;
    OsmPlacemarkData initialOsmData;
    if ( placemark()->hasOsmData() ) {
        osmData = &placemark()->osmData();
    }

    GeoDataLinearRing &outerRing = polygon->outerBoundary();
    QVector<GeoDataLinearRing> &innerRings = polygon->innerBoundaries();

    const int outerIndex = outerNodeContains( mouseEvent->pos() );
    // If the selected node is an outer boundary node.
    if ( outerIndex != -1 ) {
        // If this is the first node selected to be merged.
        if ( m_firstMergedNode.first == -1 && m_firstMergedNode.second == -1 ) {
            m_firstMergedNode = QPair<int, int>( outerIndex, -1 );
            m_outerNodesList[outerIndex].setFlag( PolylineNode::NodeIsMerged );
        // If the first selected node was an inner boundary node, raise the request for showing
        // warning.
        } else if ( m_firstMergedNode.first != -1 && m_firstMergedNode.second != -1 ) {
            setRequest( SceneGraphicsItem::OuterInnerMergingWarning );
            m_innerNodesList[m_firstMergedNode.first][m_firstMergedNode.second].setFlag(
                                                        PolylineNode::NodeIsMerged, false );

            if ( m_hoveredNode.first != -1 ) {
                // We can be sure that the hovered node is an outer node.
                Q_ASSERT( m_hoveredNode.second == -1 );
                m_outerNodesList[m_hoveredNode.first].setFlag( PolylineNode::NodeIsMergingHighlighted, false );
            }

            m_hoveredNode = m_firstMergedNode = QPair<int, int>( -1, -1 );
        } else {
            Q_ASSERT( m_firstMergedNode.first != -1 && m_firstMergedNode.second == -1 );

            // Clicking two times the same node results in unmarking it for merging.
            if ( m_firstMergedNode.first == outerIndex ) {
                m_outerNodesList[outerIndex].setFlag( PolylineNode::NodeIsMerged, false );
                m_firstMergedNode = QPair<int, int>( -1, -1 );
                return true;
            }

            // If two nodes which form a triangle are merged, the whole triangle should be
            // destroyed.
            if ( outerRing.size() <= 3 ) {
                setRequest( SceneGraphicsItem::RemovePolygonRequest );
                return true;
            }
            GeoDataCoordinates mergedNode = outerRing.at(m_firstMergedNode.first).interpolate( outerRing.at(outerIndex),
                                                                                                                 0.5 );
            // Keeping the osm data synchronized with the geometry
            if ( osmData ) {
                osmData->memberReference( -1 ).changeNodeReference( outerRing.at( outerIndex ), mergedNode );
                osmData->memberReference( -1 ).removeNodeReference( outerRing.at( m_firstMergedNode.first ) );
            }

            outerRing[outerIndex] = mergedNode;
            outerRing.remove( m_firstMergedNode.first );
            if ( !isValidPolygon() ) {
                if ( osmData ) {
                    placemark()->setOsmData( initialOsmData );
                }
                polygon->outerBoundary() = initialOuterRing;
                m_outerNodesList[m_firstMergedNode.first].setFlag( PolylineNode::NodeIsMerged,  false );

                // Remove highlight effect before showing warning
                if ( m_hoveredNode.first != -1 ) {
                    m_outerNodesList[m_hoveredNode.first].setFlag( PolylineNode::NodeIsMergingHighlighted, false );
                }

                m_hoveredNode = m_firstMergedNode = QPair<int, int>( -1, -1 );
                setRequest( SceneGraphicsItem::InvalidShapeWarning );
                return true;
            }

            // Do not modify it here. The animation has access to the object. It will modify the polygon.
            polygon->outerBoundary() = initialOuterRing;

            m_outerNodesList[outerIndex].setFlag( PolylineNode::NodeIsMerged );
            m_secondMergedNode = QPair<int, int>( outerIndex, -1 );

            delete m_animation;
            m_animation = new MergingPolygonNodesAnimation( this );
            setRequest( SceneGraphicsItem::StartPolygonAnimation );
        }

        return true;
    }

    // If the selected node is an inner boundary node.
    const QPair<int, int> innerIndexes = innerNodeContains( mouseEvent->pos() );
    if ( innerIndexes.first != -1 && innerIndexes.second != -1 ) {
        const int i = m_firstMergedNode.first;
        const int j = m_firstMergedNode.second;

        // If this is the first selected node.
        if ( i == -1 && j == -1 ) {
            m_firstMergedNode = innerIndexes;
            m_innerNodesList[innerIndexes.first][innerIndexes.second].setFlag( PolylineNode::NodeIsMerged );
        // If the first selected node has been an outer boundary one, raise the request for showing warning.
        } else if ( i != -1 && j == -1 ) {
            setRequest( SceneGraphicsItem::OuterInnerMergingWarning );
            m_outerNodesList[i].setFlag( PolylineNode::NodeIsMerged, false );

            if ( m_hoveredNode.first != -1 ) {
                // We can now be sure that the highlighted node is a node from polygon's outer boundary
                Q_ASSERT( m_hoveredNode.second != -1 );
                m_outerNodesList[m_hoveredNode.first].setFlag( PolylineNode::NodeIsMergingHighlighted, false );
            }
            m_firstMergedNode = QPair<int, int>( -1, -1 );
        } else {
            Q_ASSERT( i != -1 && j != -1 );
            if ( i != innerIndexes.first ) {
                setRequest( SceneGraphicsItem::InnerInnerMergingWarning );
                m_innerNodesList[i][j].setFlag( PolylineNode::NodeIsMerged, false );

                if ( m_hoveredNode.first != -1 && m_hoveredNode.second != -1 ) {
                    m_innerNodesList[m_hoveredNode.first][m_hoveredNode.second].setFlag(
                                                        PolylineNode::NodeIsMergingHighlighted, false );
                }

                m_hoveredNode = m_firstMergedNode = QPair<int, int>( -1, -1 );
                return true;
            }

            // Clicking two times the same node results in unmarking it for merging.
            if ( m_firstMergedNode == innerIndexes ) {
                m_innerNodesList[i][j].setFlag( PolylineNode::NodeIsMerged, false );
                m_firstMergedNode = QPair<int, int>( -1, -1 );
                return true;
            }

            // If two nodes which form an inner boundary of a polygon with a size smaller than
            // 3 are merged, remove the whole inner boundary.
            if ( innerRings.at(i).size() <= 3 ) {
                innerRings.remove( i );
                m_innerNodesList.removeAt( i );

                m_firstMergedNode = m_secondMergedNode = m_hoveredNode = QPair<int, int>( -1, -1 );
                return true;
            }

            m_innerNodesList[innerIndexes.first][innerIndexes.second].setFlag( PolylineNode::NodeIsMerged );
            m_secondMergedNode = innerIndexes;

            m_animation = new MergingPolygonNodesAnimation( this );
            setRequest( SceneGraphicsItem::StartPolygonAnimation );
        }

        return true;
    }

    return false;
}

bool AreaAnnotation::processMergingOnMove( QMouseEvent *mouseEvent )
{
    return dealWithHovering( mouseEvent );
}

bool AreaAnnotation::processMergingOnRelease( QMouseEvent *mouseEvent )
{
    Q_UNUSED( mouseEvent );
    return true;
}

bool AreaAnnotation::processAddingNodesOnPress( QMouseEvent *mouseEvent )
{
    if ( mouseEvent->button() != Qt::LeftButton ) {
        return false;
    }

    GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark()->geometry() );
    GeoDataLinearRing &outerRing = polygon->outerBoundary();
    QVector<GeoDataLinearRing> &innerRings = polygon->innerBoundaries();

    // If a virtual node has just been clicked, add it to the polygon's outer boundary
    // and start 'adjusting' its position.
    const QPair<int, int> index = virtualNodeContains( mouseEvent->pos() );
    if ( index != QPair<int, int>( -1, -1 ) && m_adjustedNode == -2 ) {
        Q_ASSERT( m_virtualHovered == index );
        const int i = index.first;
        const int j = index.second;

        if ( i != -1 && j == -1 ) {
            GeoDataLinearRing newRing( Tessellate );
            QVector<PolylineNode> newList;
            newList.reserve(outerRing.size());
            for ( int k = i; k < i + outerRing.size(); ++k ) {
                newRing.append( outerRing.at(k % outerRing.size()) );

                PolylineNode newNode;
                newNode.setFlags( m_outerNodesList.at(k % outerRing.size()).flags() );
                newList.append( newNode );
            }
            GeoDataCoordinates newCoords = newRing.first().interpolate( newRing.last(), 0.5 );
            newRing.append( newCoords );

            m_outerNodesList = newList;
            m_outerNodesList.append( PolylineNode( QRegion() ) );

            polygon->outerBoundary() = newRing;
            m_adjustedNode = -1;
        } else {
            Q_ASSERT( i != -1 && j != -1 );

            GeoDataLinearRing newRing( Tessellate );
            QVector<PolylineNode> newList;
            newList.reserve(innerRings.at(i).size());
            for ( int k = j; k < j + innerRings.at(i).size(); ++k ) {
                newRing.append( innerRings.at(i).at(k % innerRings.at(i).size()) );

                PolylineNode newNode;
                newNode.setFlags( m_innerNodesList.at(i).at(k % innerRings.at(i).size()).flags() );
                newList.append( newNode );
            }
            GeoDataCoordinates newCoords = newRing.first().interpolate( newRing.last(), 0.5 );
            newRing.append( newCoords );

            m_innerNodesList[i] = newList;
            m_innerNodesList[i].append( PolylineNode( QRegion() ) );

            polygon->innerBoundaries()[i] = newRing;
            m_adjustedNode = i;
        }

        m_virtualHovered = QPair<int, int>( -1, -1 );
        return true;
    }

    // If a virtual node which has been previously clicked and selected to become a
    // 'real node' is clicked one more time, it stops from being 'adjusted'.
    const int outerIndex = outerNodeContains( mouseEvent->pos() );
    if ( outerIndex != -1 && m_adjustedNode != -2 ) {
        m_adjustedNode = -2;
        return true;
    }

    const QPair<int,int> innerIndex = innerNodeContains( mouseEvent->pos() );
    if ( innerIndex != QPair<int, int>( -1, -1 ) && m_adjustedNode != -2 ) {
        m_adjustedNode = -2;
        return true;
    }

    return false;
}

bool AreaAnnotation::processAddingNodesOnMove( QMouseEvent *mouseEvent )
{
    Q_ASSERT( mouseEvent->button() == Qt::NoButton );

    const QPair<int, int> index = virtualNodeContains( mouseEvent->pos() );
    // If we are adjusting a virtual node which has just been clicked and became real, just
    // change its coordinates when moving it, as we do with nodes in Editing state on move.
    if ( m_adjustedNode != -2 ) {
        // The virtual node which has just been added is always the last within
        // GeoDataLinearRing's container.qreal lon, lat;
        qreal lon, lat;
        m_viewport->geoCoordinates( mouseEvent->pos().x(),
                                    mouseEvent->pos().y(),
                                    lon, lat,
                                    GeoDataCoordinates::Radian );
        const GeoDataCoordinates newCoords( lon, lat );
        GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark()->geometry() );

        if ( m_adjustedNode == -1 ) {
            polygon->outerBoundary().last() = newCoords;
        } else {
            Q_ASSERT( m_adjustedNode >= 0 );
            polygon->innerBoundaries()[m_adjustedNode].last() = newCoords;
        }

        return true;

    // If we are hovering a virtual node, store its index in order to be painted in drawNodes
    // method.
    } else if ( index != QPair<int, int>( -1, -1 ) ) {
        m_virtualHovered = index;
        return true;
    }

    // This means that the interior of the polygon has been hovered. Let the event propagate
    // since there may be overlapping polygons.
    return false;
}

bool AreaAnnotation::processAddingNodesOnRelease( QMouseEvent *mouseEvent )
{
    Q_UNUSED( mouseEvent );
    return m_adjustedNode == -2;
}

bool AreaAnnotation::dealWithHovering( QMouseEvent *mouseEvent )
{
    const PolylineNode::PolyNodeFlag flag = state() == SceneGraphicsItem::Editing ?
                                                       PolylineNode::NodeIsEditingHighlighted :
                                                       PolylineNode::NodeIsMergingHighlighted;

    const int outerIndex = outerNodeContains( mouseEvent->pos() );
    if ( outerIndex != -1 ) {
        if ( !m_outerNodesList.at(outerIndex).isEditingHighlighted() &&
             !m_outerNodesList.at(outerIndex).isMergingHighlighted() ) {
            // Deal with the case when two nodes are very close to each other.
            if ( m_hoveredNode != QPair<int, int>( -1, -1 ) ) {
                const int i = m_hoveredNode.first;
                const int j = m_hoveredNode.second;

                if ( j == -1 ) {
                    m_outerNodesList[i].setFlag( flag, false );
                } else {
                    m_innerNodesList[i][j].setFlag( flag, false );
                }
            }

            m_hoveredNode = QPair<int, int>( outerIndex, -1 );
            m_outerNodesList[outerIndex].setFlag( flag );
            setRequest( ChangeCursorPolygonNodeHover );
        }

        return true;
    } else if ( m_hoveredNode != QPair<int, int>( -1, -1 ) && m_hoveredNode.second == -1 ) {
        m_outerNodesList[m_hoveredNode.first].setFlag( flag, false );
        m_hoveredNode = QPair<int, int>( -1, -1 );

        return true;
    }

    const QPair<int, int> innerIndex = innerNodeContains( mouseEvent->pos() );
    if ( innerIndex != QPair<int, int>( -1, -1 ) ) {
        if ( !m_innerNodesList.at(innerIndex.first).at(innerIndex.second).isEditingHighlighted() &&
             !m_innerNodesList.at(innerIndex.first).at(innerIndex.second).isMergingHighlighted()) {
            // Deal with the case when two nodes are very close to each other.
            if ( m_hoveredNode != QPair<int, int>( -1, -1 ) ) {
                const int i = m_hoveredNode.first;
                const int j = m_hoveredNode.second;

                if ( j == -1 ) {
                    m_outerNodesList[i].setFlag( flag, false );
                } else {
                    m_innerNodesList[i][j].setFlag( flag, false );
                }
            }

            m_hoveredNode = innerIndex;
            m_innerNodesList[innerIndex.first][innerIndex.second].setFlag( flag );
            setRequest( ChangeCursorPolygonNodeHover );
        }

        return true;
    } else if ( m_hoveredNode != QPair<int, int>( -1, -1 ) && m_hoveredNode.second != -1 ) {
        m_innerNodesList[m_hoveredNode.first][m_hoveredNode.second].setFlag( flag, false );
        m_hoveredNode = QPair<int, int>( -1, -1 );

        return true;
    }

    // This means that the interior of the polygon has been covered so we catch this event too.
    setRequest( ChangeCursorPolygonBodyHover );
    return true;
}

}
