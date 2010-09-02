//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RoutingInstruction.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>

#include <cmath>

namespace Marble
{

RoutingInstruction::RoutingInstruction( const RoutingWaypoint &item ) :
        m_roadName( item.roadName() ), m_secondsLeft( item.secondsRemaining() ),
        m_angleToPredecessor( 0.0 ), m_roundaboutExit( 0 ),
        m_predecessor( 0 ), m_successor( 0 )
{
    m_points.append( item );
}

bool RoutingInstruction::append( const RoutingWaypoint &item )
{
    m_points.push_back( item );

    if ( item.junctionType() == RoutingWaypoint::Roundabout ) {
        ++m_roundaboutExit;
        return true;
    }

    return item.roadName() == roadName();
}

QString RoutingInstruction::roadName() const
{
    return m_roadName;
}

int RoutingInstruction::secondsLeft() const
{
    return m_secondsLeft;
}

void RoutingInstruction::calculateAngle()
{
    if ( !m_predecessor ) {
        return;
    }

    int hisSize = m_predecessor->points().size();
    int mySize = m_points.size();
    Q_ASSERT( mySize > 0 && hisSize > 0 );
    RoutingPoint one = points().first().point();
    RoutingPoint two = m_predecessor->points().at( hisSize - 1 ).point();
    qreal distance = 0;
    for ( int i = 2; i <= qMin<int>( hisSize, 20 ) && distance < 50.0; ++i ) {
        two = m_predecessor->points().at( hisSize - i ).point();
        m_intersectionPoints.push_front( two );
        distance = one.distance( two );
    }
    qreal before = two.bearing( one );
    m_intersectionPoints.push_back( one );

    one = points().first().point();
    if ( mySize == 1 && !m_successor ) {
        return;
    } else if ( mySize == 1 ) {
        Q_ASSERT( !m_successor->points().isEmpty() );
        two = m_successor->points().first().point();
    } else {
        two = points().at( 1 ).point();
    }

    distance = 0;
    m_intersectionPoints.push_back( one );
    for ( int i = 2; i < qMin<int>( mySize, 20 ) && distance < 50.0; ++i ) {
        two = points().at( i ).point();
        m_intersectionPoints.push_back( two );
        distance = one.distance( two );
    }

    qreal after = one.bearing( two );
    m_angleToPredecessor = after - before;
}

void RoutingInstruction::calculateTurnType()
{
    if ( predecessor() && predecessor()->roundaboutExitNumber() ) {
        int exit = predecessor()->roundaboutExitNumber();
        switch( exit ) {
        case 1:
            m_turnType = RoundaboutFirstExit;
            break;
        case 2:
            m_turnType = RoundaboutSecondExit;
            break;
        case 3:
            m_turnType = RoundaboutThirdExit;
            break;
        default:
            m_turnType = RoundaboutExit;
            break;
        }

        return;
    }

    int angle = qRound( angleToPredecssor() * 180.0 / M_PI + 540 ) % 360;
    Q_ASSERT( angle >= 0 && angle <= 360 );

    const int sharp = 30;
    if ( angle >= 360 - sharp || angle < sharp ) {
        m_turnType = TurnAround;
    } else if ( angle >= sharp && angle < 90 - sharp ) {
        m_turnType = SharpLeft;
    } else if ( angle >= 90 - sharp && angle < 90 + sharp ) {
        m_turnType = Left;
    } else if ( angle >= 90 + sharp && angle < 180 - sharp ) {
        m_turnType = SlightLeft;
    } else if ( angle >= 180 - sharp && angle < 180 + sharp ) {
        m_turnType = Straight;
    } else if ( angle >= 180 + sharp && angle < 270 - sharp ) {
        m_turnType = SlightRight;
    } else if ( angle >= 270 - sharp && angle < 270 + sharp ) {
        m_turnType = Right;
    } else if ( angle >= 270 + sharp && angle < 360 - sharp ) {
        m_turnType = SharpRight;
    } else {
        Q_ASSERT( false && "Internal error: not all angles are properly handled" );
    }
}

QVector<RoutingWaypoint> RoutingInstruction::points() const
{
    return m_points;
}

QVector<RoutingPoint> RoutingInstruction::intersectionPoints() const
{
    return m_intersectionPoints;
}

qreal RoutingInstruction::angleToPredecssor() const
{
    return m_angleToPredecessor;
}

RoutingInstruction* RoutingInstruction::predecessor()
{
    return m_predecessor;
}

const RoutingInstruction* RoutingInstruction::predecessor() const
{
    return m_predecessor;
}

void RoutingInstruction::setPredecessor( RoutingInstruction* predecessor )
{
    m_predecessor = predecessor;
    calculateAngle();
    calculateTurnType();
}

RoutingInstruction* RoutingInstruction::successor()
{
    return m_successor;
}

const RoutingInstruction* RoutingInstruction::successor() const
{
    return m_successor;
}

void RoutingInstruction::setSuccessor( RoutingInstruction* successor )
{
    m_successor = successor;
}

qreal RoutingInstruction::distance() const
{
    qreal result = 0.0;
    for ( int i = 1; i < m_points.size(); ++i ) {
        result += m_points[i-1].point().distance( m_points[i].point() );
    }

    return result;
}

qreal RoutingInstruction::distanceFromStart() const
{
    qreal result = 0.0;
    const RoutingInstruction* i = predecessor();
    while ( i ) {
        result += i->distance();
        i = i->predecessor();
    }
    return result;
}

qreal RoutingInstruction::distanceToEnd() const
{
    qreal result = distance();
    const RoutingInstruction* i = successor();
    while ( i ) {
        result += i->distance();
        i = i->successor();
    }
    return result;
}

QString RoutingInstruction::nextRoadInstruction() const
{
    if ( predecessor() && predecessor()->roundaboutExitNumber() ) {
        QString text = QObject::tr( "Take the %1. exit in the roundabout into %2." );
        int exit = predecessor()->roundaboutExitNumber();
        return text.arg( exit ).arg( roadName() );
    }

    switch( m_turnType ) {
    case TurnAround:
        return QObject::tr( "Turn around onto %1." ).arg( roadName() );
    case SharpLeft:
        return QObject::tr( "Turn sharp left on %1." ).arg( roadName() );
    case Left:
        return QObject::tr( "Turn left into %1." ).arg( roadName() );
    case SlightLeft:
        return QObject::tr( "Keep slightly left on %1." ).arg( roadName() );
    case Straight:
        return QObject::tr( "Continue on %1." ).arg( roadName() );
    case SlightRight:
        return QObject::tr( "Keep slightly right on %1." ).arg( roadName() );
    case Right:
        return QObject::tr( "Turn right into %1." ).arg( roadName() );
    case SharpRight:
        return QObject::tr( "Turn sharp right into %1." ).arg( roadName() );
    case Unknown:
    case RoundaboutExit:
    case RoundaboutFirstExit:
    case RoundaboutSecondExit:
    case RoundaboutThirdExit:
        Q_ASSERT( false && "Internal error: Unknown/Roundabout should have been handled earlier." );
        return QString();
        break;
    }

    Q_ASSERT( false && "Internal error: Switch did not handle all cases.");
    return QString();
}

QString RoutingInstruction::nextDistanceInstruction() const
{
    QString distanceUnit = "m";
    int precision = 0;
    qreal length = distance();
    if ( length >= 1000 ) {
        length /= 1000;
        distanceUnit = "km";
        precision = 1;
    } else if ( length >= 200 ) {
        length = 50 * qRound( length / 50 );
    } else if ( length >= 100 ) {
        length = 25 * qRound( length / 25 );
    } else {
        length = 10 * qRound( length / 10 );
    }

    if ( length == 0 ) {
        return QString();
    } else {
        QString text = QObject::tr( "Follow the road for %1 %2." );
        return text.arg( length, 0, 'f', precision ).arg( distanceUnit );
    }
}

QString RoutingInstruction::totalDurationRemaining() const
{
    qreal duration = secondsLeft();
    QString durationUnit = "sec";
    int precision = 0;
    if ( duration >= 60.0 ) {
        duration /= 60.0;
        durationUnit = "min";
        precision = 0;
    }
    if ( duration >= 60.0 ) {
        duration /= 60.0;
        durationUnit = "h";
        precision = 1;
    }

    QString text = "Arrival in %1 %2.";
    return text.arg( duration, 0, 'f', precision ).arg( durationUnit );
}

QString RoutingInstruction::instructionText() const
{
    QString text = nextRoadInstruction();
    text += " " + nextDistanceInstruction();
    if ( QCoreApplication::instance()->arguments().contains( "--remaining-duration" ) ) {
        text += " " + totalDurationRemaining();
    }
    return text;
}

QTextStream& operator<<( QTextStream& stream, const RoutingInstruction &i )
{
    stream.setRealNumberPrecision( 8 );
    if ( i.points().isEmpty() ) {
        return stream;
    }

    if ( QCoreApplication::instance()->arguments().contains( "--dense" ) ) {
        QVector<RoutingWaypoint> points = i.points();
        int maxElement = points.size() - ( i.successor() ? 1 : 0 );
        for ( int j = 0; j < maxElement; ++j ) {
            stream << points[j].point().lat() << ',';
            stream << points[j].point().lon() << ',';
            stream << points[j].junctionType() << ',';
            stream << points[j].roadType() << ',';
            stream << points[j].secondsRemaining() << ',';
            if ( !j ) {
                stream << i.instructionText();
            }
            if ( j < maxElement - 1 ) {
                stream << '\n';
            }
        }

        return stream;
    }

    if ( QCoreApplication::instance()->arguments().contains( "--csv" ) ) {
        stream << i.points().first().point().lat() << ',';
        stream << i.points().first().point().lon() << ',';
    } else {
        QString distanceUnit = "m ";
        int precision = 0;
        qreal length = i.distanceFromStart();
        if ( length >= 1000 ) {
            length /= 1000;
            distanceUnit = "km";
            precision = 1;
        }

        QString totalDistance = "[%1 %2] ";
        stream << totalDistance.arg( length, 3, 'f', precision ).arg( distanceUnit );
    }

    stream << i.instructionText();

    if ( QCoreApplication::instance()->arguments().contains( "--csv" ) && QCoreApplication::instance()->arguments().contains( "--intersection-points" ) ) {
        foreach( const RoutingPoint &point, i.intersectionPoints() ) {
            stream << ',' << point.lat() << ',' << point.lon();
        }
    }

    return stream;
}

int RoutingInstruction::roundaboutExitNumber() const
{
    return m_roundaboutExit;
}

RoutingInstruction::TurnType RoutingInstruction::turnType() const
{
    return m_turnType;
}

} // namespace Marble
