//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_ROUTINGINSTRUCTION_H
#define MARBLE_ROUTINGINSTRUCTION_H

#include "RoutingWaypoint.h"
#include "RoutingPoint.h"
#include "marble_export.h"

#include <QVector>
#include <QMetaType>

class QTextStream;

namespace Marble
{

/**
  * Stores data related to one instruction: Road name, angle to predecessor,
  * associated waypoints etc. Can be streamed directly to a QTextStream.
  */
class MARBLE_EXPORT RoutingInstruction
{
public:
    enum TurnType {
        Unknown = 0,
        Continue = 13 /** Continue on the next street */,
        Merge = 14,
        Straight = 1,
        SlightRight = 2,
        Right = 3,
        SharpRight = 4,
        TurnAround = 5 /** Perform a u-turn */,
        SharpLeft = 6,
        Left = 7,
        SlightLeft = 8,
        RoundaboutFirstExit = 9 /** Enter the roundabout and leave at the first exit */,
        RoundaboutSecondExit = 10 /** Enter the roundabout and leave at the second exit */,
        RoundaboutThirdExit = 11 /** Enter the roundabout and leave at the third exit */,
        RoundaboutExit = 12 /** At the point where the roundabout should be exited */,
        ExitLeft = 15,
        ExitRight = 16
    };

    /** Constructor */
    explicit RoutingInstruction( const RoutingWaypoint &item = RoutingWaypoint() );

    /**
      * Append data of the given item, returns true if item's street
      * name matches instructions street name
      */
    bool append( const RoutingWaypoint &item, int angle );

    /** Name of the road to turn into */
    QString roadName() const;

    /** OSM type of the road to turn into */
    QString roadType() const;

    /** Estimated number of seconds to the route destination */
    int secondsLeft() const;

    /** Waypoints from the last instruction to this instruction */
    QVector<RoutingWaypoint> points() const;

    /**
      * Contains the intersection point and points near it on the previous and current road.
      * Near is taken as that waypoint with the largest different to the intersection point
      * that does not exceed 50 meter.
      */
    QVector<RoutingPoint> intersectionPoints() const;

    /** The angle between the two turn roads, in radians */
    qreal angleToPredecssor() const;

    /** Previous turn road. Will be 0 for the first one (route start) */
    RoutingInstruction* predecessor();

    /** Const overload for #predecessor */
    const RoutingInstruction* predecessor() const;

    /** Change the predecessor */
    void setPredecessor( RoutingInstruction* predecessor );

    /** Next turn road. Will be 0 for the last one (destination) */
    RoutingInstruction* successor();

    /** Const overload for #successor */
    const RoutingInstruction* successor() const;

    /** Change the successor */
    void setSuccessor( RoutingInstruction* successor );

    /** The accumulated distance of all waypoints belonging to this instruction */
    qreal distance() const;

    /** The distance from the route start */
    qreal distanceFromStart() const;

    /** The distance to the route end. Includes the own distance */
    qreal distanceToEnd() const;

    TurnType turnType() const;

    /** Formats the instruction (road name) for a human reader */
    QString nextRoadInstruction() const;

    /** Formats the instruction (distance to next instruction) for a human reader */
    QString nextDistanceInstruction() const;

    /** Formats the instruction (duration to destination) for a human reader */
    QString totalDurationRemaining() const;

    /** Formats the instruction for a human reader */
    QString instructionText() const;

    static QString generateRoadInstruction( TurnType turnType, const QString &roadName );

protected:
    int roundaboutExitNumber() const;

private:
    void calculateAngle();

    void calculateTurnType();

    QVector<RoutingWaypoint> m_points;

    QVector<RoutingPoint> m_intersectionPoints;

    QString m_roadName;

    QString m_roadType;

    int m_secondsLeft;

    qreal m_angleToPredecessor;

    TurnType m_turnType;

    int m_roundaboutExit;

    RoutingInstruction* m_predecessor;

    RoutingInstruction* m_successor;
};

QTextStream& operator<<( QTextStream& stream, const RoutingInstruction &i );

using RoutingInstructions = QVector<RoutingInstruction>;

} // namespace Marble

Q_DECLARE_METATYPE( Marble::RoutingInstruction::TurnType )

#endif // MARBLE_ROUTINGINSTRUCTION_H
