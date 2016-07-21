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

#ifndef AREAANNOTATION_H
#define AREAANNOTATION_H

#include <QPointer>

#include "SceneGraphicsItem.h"
#include "GeoDataCoordinates.h"


namespace Marble
{

class PolylineNode;
class MergingPolygonNodesAnimation;

/**
 * @brief The AreaAnnotation class controls everything related to Polygons Editing Mode.
 * It includes polygons actions implementation and, at the same time, deals with painting
 * polygons on the map according to user's preference or to some particular states.
 */
class AreaAnnotation : public SceneGraphicsItem
{
    friend class MergingPolygonNodesAnimation;

public:
    explicit AreaAnnotation( GeoDataPlacemark *placemark );

    ~AreaAnnotation();

    /**
     * @brief Paints the nodes on the screen and updates the regions which correspond
     * to each node using the given GeoPainter.
     */
    virtual void paint( GeoPainter *painter, const ViewportParams *viewport, const QString &layer );

    /**
     * @brief Returns true if the given QPoint is contained by the current polygon. Note
     * that the return value depends on the state (e.g. in the AddingPolylineNodes state a
     * point is considered to be contained by the polygon if the virtual nodes or the
     * polygon's interior contain it, while in the Editing state, it is contained by the
     * polygon if either polygon's interior, the outer nodes or the inner nodes contain
     * it).
     */
    virtual bool containsPoint( const QPoint &point ) const;

    /**
     * @brief It is used so far to remove the hover effect while being in the
     * AddingPolylineNodes state (@see SceneGraphicsItem::dealWithItemChange documentation).
     */
    virtual void dealWithItemChange( const SceneGraphicsItem *other );

    /**
     * @brief Moves the whole polygon to the destination coordinates.
     */
    virtual void move( const GeoDataCoordinates &source, const GeoDataCoordinates &destination );

    /**
     * @brief Changes the busy state of the object according to @param enabled. It is mostly
     * used by Annotate Plugin to not send events to this object anymore but is different than
     * the focus property (you can have a focused polygon which does not accept events because
     * is busy).
     */
    void setBusy( bool enabled );

    /**
     * @brief Returns whether the annotation is 'busy' or not - this usually means that something
     * is being performed and it does not accept events anymore.
     */
    bool isBusy() const;

    /**
     * @brief Iterates through all nodes which form the polygon's outer boundary as well
     * as all its inner boundaries and sets the IsSelected flag to false.
     */
    void deselectAllNodes();

    /**
     * @brief Iterates through all nodes which form the polygon's outer boundary as well
     * as all its inner boundaries and deletes the selected ones.
     */
    void deleteAllSelectedNodes();

    /**
     * @brief Deletes the last clicked node while being in the Editing state.
     */
    void deleteClickedNode();

    /**
     * @brief If the last clicked node is selected, set its IsSelected flag to false and
     * vice versa.
     */
    void changeClickedNodeSelection();

    /**
     * @brief Tests if there are any selected nodes.
     */
    bool hasNodesSelected() const;

    /**
     * @brief Tests if the last clicked node is selected.
     */
    bool clickedNodeIsSelected() const;

    /**
     * @brief Returns the animation to be handled by a QObject which can connect signals
     * and slots.
     */
    QPointer<MergingPolygonNodesAnimation> animation();

    /**
     * @brief Provides information for downcasting a SceneGraphicsItem.
     */
    virtual const char *graphicType() const;

protected:
    /**
     * @brief Protected methods which handle mouse events and are called by
     * SceneGraphicsItem::sceneEvent() (@see Template Method pattern). Each of these
     * event handlers are structured according to the state.
     */
    virtual bool mousePressEvent( QMouseEvent *event );
    virtual bool mouseMoveEvent( QMouseEvent *event );
    virtual bool mouseReleaseEvent( QMouseEvent *event );

    /**
     * @brief Protected method which applies the Polygons modifications when changing
     * states.
     */
    virtual void dealWithStateChange( SceneGraphicsItem::ActionState previousState );

private:
    /**
     * @brief Returns true if the Polygon has a valid shape (so far, the only times when
     * it could have an invalid shape would be when deleting/merging nodes from its outer
     * boundary and it would not contains anymore all the nodes which form its inner
     * boundaries).
     */
    bool isValidPolygon() const;

    /**
     * @brief It is called when the ::paint method is called for the first time. It
     * initializes the m_outerNodesList by creating the PolylineNodes.
     * @see updateRegions() method for more detailed explanation.
     */
    void setupRegionsLists( GeoPainter *painter );

    /**
     * @brief As briefly mentioned above, the PolylineNodes instances are not created at
     * each ::paint call, but only at its first call. Every time the ::paint method is
     * called after that, each node from the lists of PolylineNodes gets its setRegion()
     * method called. We need the GeoPainter for doing this because we have to get the
     * ellipse around the GeoDataCoordinates.
     */
    void updateRegions( GeoPainter *painter );

    /**
     * @brief It iterates through all nodes and paints them on the map. It takes into
     * consideration the active flags of each PolylineNode.
     */
    void drawNodes( GeoPainter *painter );

    /**
     * @brief The following functions test whether the given @p point is contained by
     * each list.
     * @return The QPair<int, int> returned by innerNodeContains is extensively used
     * within the implementation and has the following interpretation: the node which
     * contains the given point is the '.second'h node from the '.first'h inner boundary.
     */
    int outerNodeContains( const QPoint &point ) const;
    QPair<int, int> innerNodeContains( const QPoint &point ) const;
    QPair<int, int> virtualNodeContains( const QPoint &point ) const;
    int innerBoundsContain( const QPoint &point ) const;
    bool polygonContains( const QPoint &point ) const;

    /**
     * @brief It is called from processOnMove functions and deals with polygons
     * hovering.
     */
    bool dealWithHovering( QMouseEvent *mouseEvent );

    /**
     * @brief Each state has its corresponding event handler, since in each state the
     * item may behave differently. These are the event handlers for the Editing state.
     */
    bool processEditingOnPress( QMouseEvent *mouseEvent );
    bool processEditingOnMove( QMouseEvent *mouseEvent );
    bool processEditingOnRelease( QMouseEvent *mouseEvent );

    /**
     * @brief These are the event handlers for the AddingPolygonHole state.
     */
    bool processAddingHoleOnPress( QMouseEvent *mouseEvent );
    bool processAddingHoleOnMove( QMouseEvent *mouseEvent );
    bool processAddingHoleOnRelease( QMouseEvent *mouseEvent );

    /**
     * @brief These are the event handlers for the MergingPolylineNodes state.
     */
    bool processMergingOnPress( QMouseEvent *mouseEvent );
    bool processMergingOnMove( QMouseEvent *mouseEvent );
    bool processMergingOnRelease( QMouseEvent *mouseEvent );

    /**
     * @brief These are the event handlers for the AddingPolylineNodes state.
     */
    bool processAddingNodesOnPress( QMouseEvent *mouseEvent );
    bool processAddingNodesOnMove( QMouseEvent *mouseEvent );
    bool processAddingNodesOnRelease( QMouseEvent *mouseEvent );

    /**
     * @brief Since they are used in many functions, the size and color of nodes for each
     * state are static and have class scope.
     */
    static const int regularDim;
    static const int selectedDim;
    static const int mergedDim;
    static const int hoveredDim;
    static const QColor regularColor;
    static const QColor mergedColor;

    const ViewportParams *m_viewport;
    bool m_regionsInitialized;
    bool m_busy;

    QVector<PolylineNode>            m_outerNodesList;
    QVector<PolylineNode>            m_outerVirtualNodes;
    QVector< QVector<PolylineNode> > m_innerNodesList;
    QVector< QVector<PolylineNode> > m_innerVirtualNodes;
    QVector<QRegion>                 m_boundariesList;

    // Used in the Editing state
    enum EditingInteractingObject {
        InteractingNothing, // e.g. when hovering
        InteractingNode,
        InteractingPolygon
    };
    GeoDataCoordinates       m_movedPointCoords;
    QPair<int, int>          m_clickedNodeIndexes;
    QPair<int, int>          m_hoveredNode;
    EditingInteractingObject m_interactingObj;

    // Used in Merging Nodes state
    QPair<int, int>    m_firstMergedNode;
    QPair<int, int>    m_secondMergedNode;
    QPointer<MergingPolygonNodesAnimation> m_animation;

    // Used in Adding Nodes state
    QPair<int, int> m_virtualHovered;

    // It can have the following values:
    //     -> -2 - means there is no node being adjusted;
    //     -> -1 - means the node which is being adjusted is a node from polygon's
    //             outer boundary (more exactly, the last; see below);
    //     -> i  - (i >= 0) means the node which is being adjusted is a node from
    //             the i'th inner boundary (more exactly, the last one; see below).
    // Due to the way the node appending is done (by rotating the vector which
    // contains the coordinates), we can be sure that the node we want to adjust
    // is every time the last one.
    int             m_adjustedNode;
};

}

#endif
