//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Calin Cruceru  <crucerucalincristian@gmail.com>
//

#ifndef POLYLINEANNOTATION_H
#define POLYLINEANNOTATION_H

#include <QColor>
#include <QPointer>

#include "SceneGraphicsItem.h"
#include "GeoDataCoordinates.h"


namespace Marble
{

class PolylineNode;
class MergingPolylineNodesAnimation;

class PolylineAnnotation : public SceneGraphicsItem
{
    friend class MergingPolylineNodesAnimation;

public:
    explicit PolylineAnnotation( GeoDataPlacemark *placemark );
    ~PolylineAnnotation();

    /**
     * @brief Paints the nodes on the screen and updates the regions which correspond
     * to each node using the given GeoPainter.
     */
    virtual void paint( GeoPainter *painter, const ViewportParams *viewport, const QString &layer );

    /**
     * @brief Returns true if either the polyline's associated region or one of its nodes
     * contains the given QPoint. Note that the return value depends on the state.
     */
    virtual bool containsPoint( const QPoint &eventPos ) const;

    /**
     * @brief It is used so far to remove the hover effect while being in the
     * AddingPolylineNodes state (@see SceneGraphicsItem::dealWithItemChange documentation).
     */
    virtual void dealWithItemChange( const SceneGraphicsItem *other );

    /**
     * @brief Moves the whole polyline to the destination point.
     */
    virtual void move( const GeoDataCoordinates &source, const GeoDataCoordinates &destination );

    /**
     * @brief Changes the busy state of the object according to @p enabled. It is mostly used
     * by Annotate Plugin to not send events to this object anymore but is different than the
     * focus property (you can have a focused polyline which does not accept events because is
     * busy).
     */
    void setBusy( bool enabled );

    /**
     * @brief Returns whether the annotation is 'busy' or not - this usually means that something
     * is being performed and it does not accept events anymore.
     */
    bool isBusy() const;

    /**
     * @brief Iterates through all nodes which form the polyline and sets the IsSelected flag to
     * false.
     */
    void deselectAllNodes();

    /**
     * @brief Iterates through all nodes which form the polyline and deletes the selected ones.
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
    QPointer<MergingPolylineNodesAnimation> animation();

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

    virtual void dealWithStateChange( SceneGraphicsItem::ActionState previousState );

private:
    /**
    * @brief It is called when the ::paint method is called for the first time. It
    * initializes the m_nodesList by creating the PolylineNodes.
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
     * @brief Tests if the polyline's nodes contain the given point and in case they do, it
     * returns the index of the first one.
     */
    int nodeContains( const QPoint &point ) const;

    /**
     * @brief Tests if the polyline's virtual nodes contain the given point and in case they
     * do, it returns the index of the first one.
     */
    int virtualNodeContains( const QPoint &point ) const;

    /**
     * @brief Returns true/false on how the polyline (its 'lines' excepting its nodes) contain
     * the given point or not.
     */
    bool polylineContains( const QPoint &point ) const;

    /**
     * @brief It is called from processOnMove functions and deals with polylines
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

    QVector<PolylineNode> m_nodesList;
    QVector<PolylineNode> m_virtualNodesList;
    QRegion               m_polylineRegion;

    // Used in Editing state
    enum EditingInteractingObject {
        InteractingNothing, // e.g. when hovering
        InteractingNode,
        InteractingPolyline
    };
    EditingInteractingObject m_interactingObj;
    GeoDataCoordinates m_movedPointCoords;
    int m_clickedNodeIndex;
    int m_hoveredNodeIndex;

    // Used in Merging Nodes state
    QPointer<MergingPolylineNodesAnimation> m_animation;
    int m_firstMergedNode;
    int m_secondMergedNode;

    // Used in Adding Nodes state
    int m_virtualHoveredNode;
    int m_adjustedNode;
};

}

#endif
