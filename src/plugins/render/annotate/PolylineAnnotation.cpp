//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Calin Cruceru  <crucerucalincristian@gmail.com>
//

// Self
#include "PolylineAnnotation.h"

// Qt
#include <QApplication>
#include <QPalette>
#include <qmath.h>

// Marble
#include "SceneGraphicsTypes.h"
#include "GeoPainter.h"
#include "PolylineNode.h"
#include "MarbleMath.h"
#include "GeoDataLineString.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTypes.h"
#include "ViewportParams.h"
#include "MergingPolylineNodesAnimation.h"
#include "osm/OsmPlacemarkData.h"


namespace Marble
{

const int PolylineAnnotation::regularDim = 15;
const int PolylineAnnotation::selectedDim = 15;
const int PolylineAnnotation::mergedDim = 20;
const int PolylineAnnotation::hoveredDim = 20;
const QColor PolylineAnnotation::regularColor = Oxygen::aluminumGray3;
const QColor PolylineAnnotation::mergedColor = Oxygen::emeraldGreen6;

PolylineAnnotation::PolylineAnnotation( GeoDataPlacemark *placemark ) :
    SceneGraphicsItem( placemark ),
    m_viewport( 0 ),
    m_regionsInitialized( false ),
    m_busy( false ),
    m_interactingObj( InteractingNothing ),
    m_clickedNodeIndex( -1 ),
    m_hoveredNodeIndex( -1 ),
    m_virtualHoveredNode( -1 )

{
    setPaintLayers(QStringList() << "PolylineAnnotation");
}

PolylineAnnotation::~PolylineAnnotation()
{
    delete m_animation;
}

void PolylineAnnotation::paint(GeoPainter *painter, const ViewportParams *viewport , const QString &layer)
{
    Q_UNUSED(layer);
    m_viewport = viewport;
    Q_ASSERT( placemark()->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType );

    painter->save();
    if ( state() == SceneGraphicsItem::DrawingPolyline || !m_regionsInitialized ) {
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

void PolylineAnnotation::setupRegionsLists( GeoPainter *painter )
{
    Q_ASSERT( state() == SceneGraphicsItem::DrawingPolyline || !m_regionsInitialized );
    const GeoDataLineString line = static_cast<const GeoDataLineString>( *placemark()->geometry() );

    // Add poyline nodes.
    QVector<GeoDataCoordinates>::ConstIterator itBegin = line.constBegin();
    QVector<GeoDataCoordinates>::ConstIterator itEnd = line.constEnd();

    m_nodesList.clear();
    m_nodesList.reserve(line.size());
    for ( ; itBegin != itEnd; ++itBegin ) {
        const PolylineNode newNode = PolylineNode( painter->regionFromEllipse( *itBegin, regularDim, regularDim ) );
        m_nodesList.append( newNode );
    }

    // Add region from polyline so that events on polyline's 'lines' could be caught.
    m_polylineRegion = painter->regionFromPolyline( line, 15 );
}

void PolylineAnnotation::updateRegions( GeoPainter *painter )
{
    if ( m_busy ) {
        return;
    }

    const GeoDataLineString line = static_cast<const GeoDataLineString>( *placemark()->geometry() );

    if ( state() == SceneGraphicsItem::AddingNodes ) {
        // Create and update virtual nodes lists when being in the AddingPolgonNodes state, to
        // avoid overhead in other states.
        m_virtualNodesList.clear();
        for ( int i = 0; i < line.size() - 1; ++i ) {
            const QRegion newRegion( painter->regionFromEllipse( line.at(i).interpolate( line.at(i+1), 0.5 ),
                                                                 hoveredDim, hoveredDim ) );
            m_virtualNodesList.append( PolylineNode( newRegion ) );
        }
    }


    // Update the polyline region;
    m_polylineRegion = painter->regionFromPolyline( line, 15 );

    // Update the node lists.
    for ( int i = 0; i < m_nodesList.size(); ++i ) {
        const QRegion newRegion = m_nodesList.at(i).isSelected() ?
                                  painter->regionFromEllipse( line.at(i), selectedDim, selectedDim ) :
                                  painter->regionFromEllipse( line.at(i), regularDim, regularDim );
        m_nodesList[i].setRegion( newRegion );
    }
}

void PolylineAnnotation::drawNodes( GeoPainter *painter )
{
    // These are the 'real' dimensions of the drawn nodes. The ones which have class scope are used
    // to generate the regions and they are a little bit larger, because, for example, it would be
    // a little bit too hard to select nodes.
    static const int d_regularDim = 10;
    static const int d_selectedDim = 10;
    static const int d_mergedDim = 20;
    static const int d_hoveredDim = 20;

    const GeoDataLineString line = static_cast<const GeoDataLineString>( *placemark()->geometry() );

    QColor glowColor = QApplication::palette().highlightedText().color();
    glowColor.setAlpha(120);
    auto const selectedColor = QApplication::palette().highlight().color();
    auto const hoveredColor = selectedColor;

    for ( int i = 0; i < line.size(); ++i ) {
        // The order here is important, because a merged node can be at the same time selected.
        if ( m_nodesList.at(i).isBeingMerged() ) {
            painter->setBrush( mergedColor );
            painter->drawEllipse( line.at(i), d_mergedDim, d_mergedDim );
        } else if ( m_nodesList.at(i).isSelected() ) {
            painter->setBrush( selectedColor );
            painter->drawEllipse( line.at(i), d_selectedDim, d_selectedDim );

            if ( m_nodesList.at(i).isEditingHighlighted() ||
                 m_nodesList.at(i).isMergingHighlighted() ) {
                QPen defaultPen = painter->pen();
                QPen newPen;
                newPen.setWidth( defaultPen.width() + 3 );
                newPen.setColor( glowColor );

                painter->setBrush( Qt::NoBrush );
                painter->setPen( newPen );
                painter->drawEllipse( line.at(i), d_selectedDim + 2, d_selectedDim + 2 );
                painter->setPen( defaultPen );
            }
        } else {
            painter->setBrush( regularColor );
            painter->drawEllipse( line.at(i), d_regularDim, d_regularDim );

            if ( m_nodesList.at(i).isEditingHighlighted() ||
                 m_nodesList.at(i).isMergingHighlighted() ) {
                QPen defaultPen = painter->pen();
                QPen newPen;
                newPen.setWidth( defaultPen.width() + 3 );
                newPen.setColor( glowColor );

                painter->setPen( newPen );
                painter->setBrush( Qt::NoBrush );
                painter->drawEllipse( line.at(i), d_regularDim + 2, d_regularDim + 2 );
                painter->setPen( defaultPen );
            }
        }
    }

    if ( m_virtualHoveredNode != -1 ) {
        painter->setBrush( hoveredColor );

        GeoDataCoordinates newCoords;
        if ( m_virtualHoveredNode + 1 ) {
            newCoords = line.at( m_virtualHoveredNode + 1 ).interpolate( line.at( m_virtualHoveredNode ), 0.5 );
        } else {
            newCoords = line.first().interpolate( line.last(), 0.5 );
        }
        painter->drawEllipse( newCoords, d_hoveredDim, d_hoveredDim );
    }
}

bool PolylineAnnotation::containsPoint( const QPoint &point ) const
{
    if ( state() == SceneGraphicsItem::Editing ) {
        return nodeContains( point ) != -1 || polylineContains( point );
    } else if ( state() == SceneGraphicsItem::MergingNodes ) {
        return nodeContains( point ) != -1;
    } else if ( state() == SceneGraphicsItem::AddingNodes ) {
        return virtualNodeContains( point ) != -1 ||
               nodeContains( point ) != -1 ||
               polylineContains( point );
    }

    return false;
}

int PolylineAnnotation::nodeContains( const QPoint &point ) const
{
    if ( !hasFocus() ) {
        return -1;
    }

    for ( int i = 0; i < m_nodesList.size(); ++i ) {
        if ( m_nodesList.at(i).containsPoint( point ) ) {
            return i;
        }
    }

    return -1;
}

int PolylineAnnotation::virtualNodeContains( const QPoint &point ) const
{
    if ( !hasFocus() ) {
        return -1;
    }

    for ( int i = 0; i < m_virtualNodesList.size(); ++i ) {
        if ( m_virtualNodesList.at(i).containsPoint( point ) )
            return i;
    }

    return -1;
}

bool PolylineAnnotation::polylineContains( const QPoint &point ) const
{
    return m_polylineRegion.contains( point );
}

void PolylineAnnotation::dealWithItemChange( const SceneGraphicsItem *other )
{
    Q_UNUSED( other );

    // So far we only deal with item changes when hovering nodes, so that
    // they do not remain hovered when changing the item we interact with.
    if ( state() == SceneGraphicsItem::Editing ) {
        if ( m_hoveredNodeIndex != -1 &&
             m_hoveredNodeIndex < static_cast<GeoDataLineString*>( placemark()->geometry() )->size() ) {
            m_nodesList[m_hoveredNodeIndex].setFlag( PolylineNode::NodeIsEditingHighlighted, false );
        }

        m_hoveredNodeIndex = -1;
    } else if ( state() == SceneGraphicsItem::MergingNodes ) {
        if ( m_hoveredNodeIndex != -1 ) {
            m_nodesList[m_hoveredNodeIndex].setFlag( PolylineNode::NodeIsMergingHighlighted, false );
        }

        m_hoveredNodeIndex = -1;
    } else if ( state() == SceneGraphicsItem::AddingNodes ) {
        m_virtualHoveredNode = -1;
    }
}

void PolylineAnnotation::move( const GeoDataCoordinates &source, const GeoDataCoordinates &destination )
{
    GeoDataLineString *lineString = static_cast<GeoDataLineString*>( placemark()->geometry() );
    GeoDataLineString oldLineString = *lineString;
    OsmPlacemarkData *osmData = 0;
    if ( placemark()->hasOsmData() ) {
        osmData = &placemark()->osmData();
    }
    lineString->clear();

    const qreal deltaLat = destination.latitude() - source.latitude();
    const qreal deltaLon = destination.longitude() - source.longitude();

    Quaternion latRectAxis = Quaternion::fromEuler( 0, destination.longitude(), 0);
    Quaternion latAxis = Quaternion::fromEuler( -deltaLat, 0, 0);
    Quaternion lonAxis = Quaternion::fromEuler(0, deltaLon, 0);
    Quaternion rotAxis = latRectAxis * latAxis * latRectAxis.inverse() * lonAxis;

    qreal lonRotated, latRotated;
    for ( int i = 0; i < oldLineString.size(); ++i ) {
        Quaternion qpos = oldLineString.at(i).quaternion();
        qpos.rotateAroundAxis(rotAxis);
        qpos.getSpherical( lonRotated, latRotated );
        GeoDataCoordinates movedPoint( lonRotated, latRotated, 0 );
        if ( osmData ) {
            osmData->changeNodeReference( oldLineString.at( i ), movedPoint );
        }
        lineString->append( movedPoint );
    }
}

void PolylineAnnotation::setBusy( bool enabled )
{
    m_busy = enabled;

    if ( !enabled && m_animation && state() == SceneGraphicsItem::MergingNodes ) {
        if ( m_firstMergedNode != -1 && m_secondMergedNode != -1 ) {
            // Update the PolylineNodes lists after the animation has finished its execution.
            m_nodesList[m_secondMergedNode].setFlag( PolylineNode::NodeIsMergingHighlighted, false );
            m_hoveredNodeIndex = -1;

            // Remove the merging node flag and add the NodeIsSelected flag if either one of the
            // merged nodes had been selected before merging them.
            m_nodesList[m_secondMergedNode].setFlag( PolylineNode::NodeIsMerged, false );
            if ( m_nodesList[m_firstMergedNode].isSelected() ) {
                m_nodesList[m_secondMergedNode].setFlag( PolylineNode::NodeIsSelected );
            }
            m_nodesList.removeAt( m_firstMergedNode );

            m_firstMergedNode = -1;
            m_secondMergedNode = -1;
        }

        delete m_animation;
    }
}

bool PolylineAnnotation::isBusy() const
{
    return m_busy;
}

void PolylineAnnotation::deselectAllNodes()
{
    if ( state() != SceneGraphicsItem::Editing ) {
        return;
    }

    for ( int i = 0 ; i < m_nodesList.size(); ++i ) {
        m_nodesList[i].setFlag( PolylineNode::NodeIsSelected, false );
    }
}

void PolylineAnnotation::deleteAllSelectedNodes()
{
    if ( state() != SceneGraphicsItem::Editing ) {
        return;
    }

    GeoDataLineString *line = static_cast<GeoDataLineString*>( placemark()->geometry() );
    OsmPlacemarkData *osmData = 0;
    if ( placemark()->hasOsmData() ) {
        osmData = &placemark()->osmData();
    }
    for ( int i = 0; i < line->size(); ++i ) {
        if ( m_nodesList.at(i).isSelected() ) {
            if ( m_nodesList.size() <= 2 ) {
                setRequest( SceneGraphicsItem::RemovePolylineRequest );
                return;
            }
            if ( osmData ) {
                osmData->removeNodeReference( line->at( i ) );
            }
            m_nodesList.removeAt( i );
            line->remove( i );
            --i;
        }
    }
}

void PolylineAnnotation::deleteClickedNode()
{
    if ( state() != SceneGraphicsItem::Editing ) {
        return;
    }

    GeoDataLineString *line = static_cast<GeoDataLineString*>( placemark()->geometry() );
    OsmPlacemarkData *osmData = 0;
    if ( placemark()->hasOsmData() ) {
        osmData = &placemark()->osmData();
    }
    if ( m_nodesList.size() <= 2 ) {
        setRequest( SceneGraphicsItem::RemovePolylineRequest );
        return;
    }

    if ( osmData ) {
        osmData->removeMemberReference( m_clickedNodeIndex );
    }

    m_nodesList.removeAt( m_clickedNodeIndex );
    line->remove( m_clickedNodeIndex );
 }

void PolylineAnnotation::changeClickedNodeSelection()
{
    if ( state() != SceneGraphicsItem::Editing ) {
        return;
    }

    m_nodesList[m_clickedNodeIndex].setFlag( PolylineNode::NodeIsSelected,
                                             !m_nodesList[m_clickedNodeIndex].isSelected() );
}

bool PolylineAnnotation::hasNodesSelected() const
{
    for ( int i = 0; i < m_nodesList.size(); ++i ) {
        if ( m_nodesList.at(i).isSelected() ) {
            return true;
        }
    }

    return false;
}

bool PolylineAnnotation::clickedNodeIsSelected() const
{
    return m_nodesList[m_clickedNodeIndex].isSelected();
}

QPointer<MergingPolylineNodesAnimation> PolylineAnnotation::animation()
{
    return m_animation;
}

bool PolylineAnnotation::mousePressEvent( QMouseEvent *event )
{
    if ( !m_viewport || m_busy ) {
        return false;
    }

    setRequest( SceneGraphicsItem::NoRequest );

    if ( state() == SceneGraphicsItem::Editing ) {
        return processEditingOnPress( event );
    } else if ( state() == SceneGraphicsItem::MergingNodes ) {
        return processMergingOnPress( event );
    } else if ( state() == SceneGraphicsItem::AddingNodes ) {
        return processAddingNodesOnPress( event );
    }

    return false;
}

bool PolylineAnnotation::mouseMoveEvent( QMouseEvent *event )
{
    if ( !m_viewport || m_busy ) {
        return false;
    }

    setRequest( SceneGraphicsItem::NoRequest );

    if ( state() == SceneGraphicsItem::Editing ) {
        return processEditingOnMove( event );
    } else if ( state() == SceneGraphicsItem::MergingNodes ) {
        return processMergingOnMove( event );
    } else if ( state() == SceneGraphicsItem::AddingNodes ) {
        return processAddingNodesOnMove( event );
    }

    return false;
}

bool PolylineAnnotation::mouseReleaseEvent( QMouseEvent *event )
{
    if ( !m_viewport || m_busy ) {
        return false;
    }

    setRequest( SceneGraphicsItem::NoRequest );

    if ( state() == SceneGraphicsItem::Editing ) {
        return processEditingOnRelease( event );
    } else if ( state() == SceneGraphicsItem::MergingNodes ) {
        return processMergingOnRelease( event );
    } else if ( state() == SceneGraphicsItem::AddingNodes ) {
        return processAddingNodesOnRelease( event );
    }

    return false;
}

void PolylineAnnotation::dealWithStateChange( SceneGraphicsItem::ActionState previousState )
{
    // Dealing with cases when exiting a state has an effect on this item.
    if ( previousState == SceneGraphicsItem::DrawingPolyline ) {
        // nothing so far
    } else if ( previousState == SceneGraphicsItem::Editing ) {
        // Make sure that when changing the state, there is no highlighted node.
        if ( m_hoveredNodeIndex != -1 ) {
            m_nodesList[m_hoveredNodeIndex].setFlag( PolylineNode::NodeIsEditingHighlighted, false );
        }

        m_clickedNodeIndex = -1;
        m_hoveredNodeIndex = -1;
    } else if ( previousState == SceneGraphicsItem::MergingNodes ) {
        // If there was only a node selected for being merged and the state changed,
        // deselect it.
        if ( m_firstMergedNode != -1 ) {
            m_nodesList[m_firstMergedNode].setFlag( PolylineNode::NodeIsMerged, false );
        }

        // Make sure that when changing the state, there is no highlighted node.
        if ( m_hoveredNodeIndex != -1 ) {
            if ( m_hoveredNodeIndex != -1 ) {
                m_nodesList[m_hoveredNodeIndex].setFlag( PolylineNode::NodeIsEditingHighlighted, false );
            }
        }

        m_hoveredNodeIndex = -1;
        delete m_animation;
    } else if ( previousState == SceneGraphicsItem::AddingNodes ) {
        m_virtualNodesList.clear();
        m_virtualHoveredNode = -1;
        m_adjustedNode = -1;
    }

    // Dealing with cases when entering a state has an effect on this item, or
    // initializations are needed.
    if ( state() == SceneGraphicsItem::Editing ) {
        m_interactingObj = InteractingNothing;
        m_clickedNodeIndex = -1;
        m_hoveredNodeIndex = -1;
    } else if ( state() == SceneGraphicsItem::MergingNodes ) {
        m_firstMergedNode = -1;
        m_secondMergedNode = -1;
        m_hoveredNodeIndex = -1;
        m_animation = 0;
    } else if ( state() == SceneGraphicsItem::AddingNodes ) {
        m_virtualHoveredNode = -1;
        m_adjustedNode = -1;
    }
}

bool PolylineAnnotation::processEditingOnPress( QMouseEvent *mouseEvent )
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

    // First check if one of the nodes has been clicked.
    m_clickedNodeIndex = nodeContains( mouseEvent->pos() );
    if ( m_clickedNodeIndex != -1 ) {
        if ( mouseEvent->button() == Qt::RightButton ) {
            setRequest( SceneGraphicsItem::ShowNodeRmbMenu );
        } else {
            Q_ASSERT( mouseEvent->button() == Qt::LeftButton );
            m_interactingObj = InteractingNode;
        }

        return true;
    }

    // Then check if the 'interior' of the polyline has been clicked (by interior
    // I mean its lines excepting its nodes).
    if ( polylineContains( mouseEvent->pos() ) ) {
        if ( mouseEvent->button() == Qt::RightButton ) {
            setRequest( SceneGraphicsItem::ShowPolylineRmbMenu );
        } else {
            Q_ASSERT( mouseEvent->button() == Qt::LeftButton );
            m_interactingObj = InteractingPolyline;
        }

        return true;
    }

    return false;
}

bool PolylineAnnotation::processEditingOnMove( QMouseEvent *mouseEvent )
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

    if ( m_interactingObj == InteractingNode ) {
        GeoDataLineString *line = static_cast<GeoDataLineString*>( placemark()->geometry() );
        OsmPlacemarkData *osmData = 0;
        if ( placemark()->hasOsmData() ) {
            osmData = &placemark()->osmData();
        }

        // Keeping the OsmPlacemarkData synchronized with the geometry
        if ( osmData ) {
            osmData->changeNodeReference( line->at( m_clickedNodeIndex ), newCoords );
        }
        line->at(m_clickedNodeIndex) = newCoords;

        return true;
    } else if ( m_interactingObj == InteractingPolyline ) {
        GeoDataLineString *lineString = static_cast<GeoDataLineString*>( placemark()->geometry() );
        OsmPlacemarkData *osmData = 0;
        if ( placemark()->hasOsmData() ) {
            osmData = &placemark()->osmData();
        }
        const GeoDataLineString oldLineString = *lineString;
        lineString->clear();

        const qreal deltaLat = lat - m_movedPointCoords.latitude();
        const qreal deltaLon = lon - m_movedPointCoords.longitude();

        Quaternion latRectAxis = Quaternion::fromEuler( 0, lon, 0);
        Quaternion latAxis = Quaternion::fromEuler( -deltaLat, 0, 0);
        Quaternion lonAxis = Quaternion::fromEuler(0, deltaLon, 0);
        Quaternion rotAxis = latRectAxis * latAxis * latRectAxis.inverse() * lonAxis;

        qreal lonRotated, latRotated;
        for ( int i = 0; i < oldLineString.size(); ++i ) {
            Quaternion qpos = oldLineString.at(i).quaternion();
            qpos.rotateAroundAxis(rotAxis);
            qpos.getSpherical( lonRotated, latRotated );
            GeoDataCoordinates movedPoint( lonRotated, latRotated, 0 );
            if ( osmData ) {
                osmData->changeNodeReference( oldLineString.at( i ), movedPoint );
            }
            lineString->append( movedPoint );
        }

        m_movedPointCoords = newCoords;
        return true;
    }

    return dealWithHovering( mouseEvent );
}

bool PolylineAnnotation::processEditingOnRelease( QMouseEvent *mouseEvent )
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

        m_nodesList[m_clickedNodeIndex].setFlag( PolylineNode::NodeIsSelected,
                                                 !m_nodesList.at(m_clickedNodeIndex).isSelected() );
        m_interactingObj = InteractingNothing;
        return true;
    } else if ( m_interactingObj == InteractingPolyline ) {
        // Nothing special happens at polyline release.
        m_interactingObj = InteractingNothing;
        return true;
    }

    return false;
}

bool PolylineAnnotation::processMergingOnPress( QMouseEvent *mouseEvent )
{
    if ( mouseEvent->button() != Qt::LeftButton ) {
        return false;
    }

    GeoDataLineString line = static_cast<GeoDataLineString>( *placemark()->geometry() );

    const int index = nodeContains( mouseEvent->pos() );
    if ( index == -1 ) {
        return false;
    }

    // If this is the first node selected to be merged.
    if ( m_firstMergedNode == -1 ) {
        m_firstMergedNode = index;
        m_nodesList[index].setFlag( PolylineNode::NodeIsMerged );
   } else {
        Q_ASSERT( m_firstMergedNode != -1 );

        // Clicking two times the same node results in unmarking it for merging.
        if ( m_firstMergedNode == index ) {
            m_nodesList[index].setFlag( PolylineNode::NodeIsMerged, false );
            m_firstMergedNode = -1;
            return true;
        }

        // If these two nodes are the last ones remained as part of the polyline, remove
        // the whole polyline.
        if ( line.size() <= 2 ) {
            setRequest( SceneGraphicsItem::RemovePolylineRequest );
            return true;
        }
        m_nodesList[index].setFlag( PolylineNode::NodeIsMerged );
        m_secondMergedNode = index;

        delete m_animation;
        m_animation = new MergingPolylineNodesAnimation( this );
        setRequest( SceneGraphicsItem::StartPolylineAnimation );
    }

    return true;
}

bool PolylineAnnotation::processMergingOnMove( QMouseEvent *mouseEvent )
{
    return dealWithHovering( mouseEvent );
}

bool PolylineAnnotation::processMergingOnRelease( QMouseEvent *mouseEvent )
{
    Q_UNUSED( mouseEvent );
    return true;
}

bool PolylineAnnotation::processAddingNodesOnPress( QMouseEvent *mouseEvent )
{
    if ( mouseEvent->button() != Qt::LeftButton ) {
        return false;
    }

    GeoDataLineString *line = static_cast<GeoDataLineString*>( placemark()->geometry() );

    // If a virtual node has just been clicked, add it to the polyline and start 'adjusting'
    // its position.
    const int virtualIndex = virtualNodeContains( mouseEvent->pos() );
    if ( virtualIndex != -1 && m_adjustedNode == -1 ) {
        Q_ASSERT( m_virtualHoveredNode == virtualIndex );

        line->insert( virtualIndex + 1, line->at( virtualIndex ).interpolate( line->at( virtualIndex + 1 ), 0.5 ) );
        m_nodesList.insert( virtualIndex + 1, PolylineNode() );

        m_adjustedNode = virtualIndex + 1;
        m_virtualHoveredNode = -1;
        return true;
    }

    // If a virtual node which has been previously clicked and selected to become a
    // 'real node' is clicked one more time, it stops from being 'adjusted'.
    const int realIndex = nodeContains( mouseEvent->pos() );
    if ( realIndex != -1 && m_adjustedNode != -1 ) {
        m_adjustedNode = -1;
        return true;
    }

    return false;
}

bool PolylineAnnotation::processAddingNodesOnMove( QMouseEvent *mouseEvent )
{
    Q_ASSERT( mouseEvent->button() == Qt::NoButton );

    const int index = virtualNodeContains( mouseEvent->pos() );

    // If we are adjusting a virtual node which has just been clicked and became real, just
    // change its coordinates when moving it, as we do with nodes in Editing state on move.
    if ( m_adjustedNode != -1 ) {
        // The virtual node which has just been added is always the last within
        // GeoDataLinearRing's container.qreal lon, lat;
        qreal lon, lat;
        m_viewport->geoCoordinates( mouseEvent->pos().x(),
                                    mouseEvent->pos().y(),
                                    lon, lat,
                                    GeoDataCoordinates::Radian );
        const GeoDataCoordinates newCoords( lon, lat );
        GeoDataLineString *line = static_cast<GeoDataLineString*>( placemark()->geometry() );
        line->at(m_adjustedNode) = newCoords;

        return true;

    // If we are hovering a virtual node, store its index in order to be painted in drawNodes
    // method.
    } else if ( index != -1 ) {
        m_virtualHoveredNode = index;
        return true;
    }

    return false;
}

bool PolylineAnnotation::processAddingNodesOnRelease( QMouseEvent *mouseEvent )
{
    Q_UNUSED( mouseEvent );
    return m_adjustedNode == -1;
}

bool PolylineAnnotation::dealWithHovering( QMouseEvent *mouseEvent )
{
    const PolylineNode::PolyNodeFlag flag = state() == SceneGraphicsItem::Editing ?
                                                       PolylineNode::NodeIsEditingHighlighted :
                                                       PolylineNode::NodeIsMergingHighlighted;

    const int index = nodeContains( mouseEvent->pos() );
    if ( index != -1 ) {
        if ( !m_nodesList.at(index).isEditingHighlighted() &&
             !m_nodesList.at(index).isMergingHighlighted() ) {
            // Deal with the case when two nodes are very close to each other.
            if ( m_hoveredNodeIndex != -1 ) {
                m_nodesList[m_hoveredNodeIndex].setFlag( flag, false );
            }

            m_hoveredNodeIndex = index;
            m_nodesList[index].setFlag( flag );
            setRequest( ChangeCursorPolylineNodeHover );
        }

        return true;
    } else if ( m_hoveredNodeIndex != -1 ) {
        m_nodesList[m_hoveredNodeIndex].setFlag( flag, false );
        m_hoveredNodeIndex = -1;

        return true;
    }

    // This means that the interior of the polyline has been hovered so we catch this event too.
    setRequest( ChangeCursorPolylineLineHover );
    return true;
}

const char *PolylineAnnotation::graphicType() const
{
    return SceneGraphicsTypes::SceneGraphicPolylineAnnotation;
}

}
