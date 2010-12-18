//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RoutingModel.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleMath.h"
#include "GeoDataCoordinates.h"
#include "GeoDataDocument.h"
#include "GeoDataGeometry.h"
#include "GeoDataFolder.h"
#include "GeoDataParser.h"
#include "GeoDataPlacemark.h"
#include "RouteRequest.h"
#include "PositionTracking.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "global.h"
#include "routing/instructions/RoutingInstruction.h"
#include "GeoDataExtendedData.h"

#include <QtCore/QBuffer>
#include <QtCore/QPointer>
#include <QtCore/QRegExp>
#include <QtCore/QVector>
#include <QtGui/QMessageBox>
#include <QtGui/QPixmap>
#include <QtXml/QDomDocument>

namespace Marble
{

struct RouteElement {
    GeoDataCoordinates position;
    RoutingModel::RoutingItemType type;
    QString description;
    GeoDataLineString instructionPointSet;
    qreal instructionDistance;
    RoutingInstruction::TurnType turnType;

    RouteElement() : turnType( RoutingInstruction::Unknown ) {}
};

/** @todo: Eventually switch to GeoDataDocument as the underlying storage */
typedef QVector<RouteElement> RouteElements;

class RoutingModelPrivate
{
public:
    enum RouteDeviation
    {
        Unknown,
        OnRoute,
        OffRoute
    };

    RoutingModelPrivate( RouteRequest* request );

    RouteElements m_route;

    RoutingModel::Duration m_totalDuration;

    qreal m_totalDistance;
    qint32 m_totalTimeRemaining;
    qreal m_timeRemaining;
    qreal m_totalDistanceRemaining;
    int m_instructionSize;
    int m_nextInstructionIndex;
    qreal m_nextInstructionDistance;
    qreal m_currentInstructionLength;
    QPixmap m_nextInstructionPixmap;
    QPixmap m_followingInstructionPixmap;
    GeoDataCoordinates m_location;
    QString m_nextDescription;
    RouteDeviation m_deviation;
    QMap<RoutingInstruction::TurnType,QPixmap> m_turnTypePixmaps;
    PositionTracking* m_positionTracking;
    RouteRequest* m_request;
    QVector<int> m_instructionLookupTable;

    void importPlacemark( const GeoDataPlacemark *placemark );

    bool deviatedFromRoute( const GeoDataCoordinates &position, const QVector<GeoDataCoordinates> &waypoints ) const;

    void updateViaPoints( const GeoDataCoordinates &position );
};

RoutingModelPrivate::RoutingModelPrivate( RouteRequest* request )
    : m_totalDistance( 0.0 ),
      m_totalTimeRemaining( 0 ),
      m_timeRemaining( 0.0 ),
      m_totalDistanceRemaining( 0.0 ),
      m_instructionSize( 0 ),
      m_nextInstructionIndex( 0 ),
      m_nextInstructionDistance( 0.0 ),
      m_currentInstructionLength( 0.0 ),
      m_deviation( Unknown ),
      m_positionTracking( 0 ),
      m_request( request )
{
    m_turnTypePixmaps[RoutingInstruction::Unknown] = QPixmap( MarbleDirs::path( "bitmaps/routing_step.png" ) );
    m_turnTypePixmaps[RoutingInstruction::Straight] = QPixmap( ":/data/bitmaps/turn-continue.png");
    m_turnTypePixmaps[RoutingInstruction::SlightRight] = QPixmap( ":/data/bitmaps/turn-slight-right.png");
    m_turnTypePixmaps[RoutingInstruction::Right] = QPixmap( ":/data/bitmaps/turn-right.png");
    m_turnTypePixmaps[RoutingInstruction::SharpRight] = QPixmap( ":/data/bitmaps/turn-sharp-right.png");
    m_turnTypePixmaps[RoutingInstruction::TurnAround] = QPixmap( ":/data/bitmaps/turn-around.png");
    m_turnTypePixmaps[RoutingInstruction::SharpLeft] = QPixmap( ":/data/bitmaps/turn-sharp-left.png");
    m_turnTypePixmaps[RoutingInstruction::Left] = QPixmap( ":/data/bitmaps/turn-left.png");
    m_turnTypePixmaps[RoutingInstruction::SlightLeft] = QPixmap( ":/data/bitmaps/turn-slight-left.png");
    m_turnTypePixmaps[RoutingInstruction::RoundaboutFirstExit] = QPixmap( ":/data/bitmaps/turn-roundabout-first.png");
    m_turnTypePixmaps[RoutingInstruction::RoundaboutSecondExit] = QPixmap( ":/data/bitmaps/turn-roundabout-second.png");
    m_turnTypePixmaps[RoutingInstruction::RoundaboutThirdExit] = QPixmap( ":/data/bitmaps/turn-roundabout-third.png");
    m_turnTypePixmaps[RoutingInstruction::RoundaboutExit] = QPixmap( ":/data/bitmaps/turn-roundabout-far.png");
}

bool RoutingModelPrivate::deviatedFromRoute( const GeoDataCoordinates &position, const QVector<GeoDataCoordinates> &waypoints ) const
{
    /** @todo: Cache bounding box / expected next target for a quicker check */
    qreal deviation = 0.0;
    if ( m_positionTracking && m_positionTracking->accuracy().vertical > 0.0 ) {
        deviation = qMax<qreal>( m_positionTracking->accuracy().vertical, m_positionTracking->accuracy().horizontal );
    }
    qreal const threshold = ( deviation + 100.0 ) / EARTH_RADIUS;
    foreach( const GeoDataCoordinates &coordinate, waypoints ) {
        if ( distanceSphere( position, coordinate ) < threshold ) {
            return false;
        }
    }

    return true;
}

void RoutingModelPrivate::updateViaPoints( const GeoDataCoordinates &position )
{
    // Mark via points visited after approaching them in a range of 500m or less
    qreal const threshold = 500 / EARTH_RADIUS;
    for( int i=0; i<m_request->size(); ++i ) {
        if ( !m_request->visited( i ) ) {
            if ( distanceSphere( position, m_request->at( i ) ) < threshold ) {
                m_request->setVisited( i, true );
            }
        }
    }
}

void RoutingModelPrivate::importPlacemark( const GeoDataPlacemark *placemark )
{
    GeoDataGeometry* geometry = placemark->geometry();
    GeoDataLineString* lineString = dynamic_cast<GeoDataLineString*>( geometry );
    QStringList blacklist = QStringList() << "" << "Route" << "Tessellated";
    if ( !blacklist.contains( placemark->name() ) ) {
        if( lineString ) {
            RouteElement element;
            element.type = RoutingModel::Instruction;
            element.description = placemark->name();
            element.position = lineString->at( 0 );
            for( int i = 0; i<lineString->size(); ++i ) {
                element.instructionPointSet << lineString->at( i );
            }
            if ( placemark->extendedData().contains( "turnType" ) ) {
                QVariant turnType = placemark->extendedData().value( "turnType" ).value();
                // The enum value is converted to/from an int in the QVariant
                // because only a limited set of data types can be serialized with QVariant's
                // toString() method (which is used to serialize <ExtendedData>/<Data> values)
                element.turnType = RoutingInstruction::TurnType( qVariantValue<int>( turnType ) );
            }
            element.instructionDistance = element.instructionPointSet.length( EARTH_RADIUS );
            m_route.push_back( element );
        }
    }
    else if ( lineString ) {
        for ( int i=0; i<lineString->size(); ++i ) {
            RouteElement element;
            element.type = RoutingModel::WayPoint;
            element.position = lineString->at( i );
            m_route.push_back( element );
        }
    }

}

RoutingModel::RoutingModel( RouteRequest* request, MarbleModel *model, QObject *parent ) :
        QAbstractListModel( parent ), d( new RoutingModelPrivate( request ) )
{
   if( model )
    {
        d->m_positionTracking = model->positionTracking();
        QObject::connect( d->m_positionTracking, SIGNAL( gpsLocation( GeoDataCoordinates, qreal ) ),
                 this, SLOT( currentInstruction( GeoDataCoordinates, qreal ) ) );
    }
}

RoutingModel::~RoutingModel()
{
    delete d;
}

int RoutingModel::rowCount ( const QModelIndex &parent ) const
{
    return parent.isValid() ? 0 : d->m_route.size();
}

QVariant RoutingModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0 ) {
        return QString( "Instruction" );
    }

    return QAbstractListModel::headerData( section, orientation, role );
}

QVariant RoutingModel::data ( const QModelIndex & index, int role ) const
{
    if ( !index.isValid() ) {
        return QVariant();
    }

    if ( index.row() < d->m_route.size() && index.column() == 0 ) {
        RouteElement element = d->m_route.at( index.row() );
        switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return element.description;
            break;
        case Qt::DecorationRole:
            if ( element.type == Instruction ) {
                bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
                if ( smallScreen ) {
                    return d->m_turnTypePixmaps[element.turnType];
                } else {
                    return d->m_turnTypePixmaps[element.turnType].scaled( 32, 32 );
                }
            }

            return QVariant();
            break;
        case CoordinateRole:
            return QVariant::fromValue( d->m_route.at( index.row() ).position );
            break;
        case TypeRole:
            return QVariant::fromValue( d->m_route.at( index.row() ).type );
            break;
        case InstructionWayPointRole:
            return QVariant::fromValue( d->m_route.at( index.row() ).instructionPointSet );
            break;
        default:
            return QVariant();
        }
    }

    return QVariant();
}

bool RoutingModel::setCurrentRoute( GeoDataDocument* document )
{
    d->m_route.clear();

    QVector<GeoDataFolder*> folders = document->folderList();
    foreach( const GeoDataFolder *folder, folders ) {
        foreach( const GeoDataPlacemark *placemark, folder->placemarkList() ) {
            d->importPlacemark( placemark );
        }
    }

    foreach( const GeoDataPlacemark *placemark, document->placemarkList() ) {
        d->importPlacemark( placemark );
    }

    d->m_deviation = RoutingModelPrivate::Unknown;

    // Generate lookup table
    d->m_instructionLookupTable.clear();
    GeoDataLineString wayPoints, instructions;
    foreach( const RouteElement &element, d->m_route ) {
        if ( element.type == WayPoint ) {
            wayPoints << element.position;
        } else if ( element.type == Instruction ) {
          instructions << element.position;
        }
    }

    int const end = wayPoints.size();
    int j=0;
    for ( int i=0; i<end; ++i ) {
      if ( j >= instructions.size() ) {
        // Towards the end of the route
        break;
      }

      if ( wayPoints.at(i) == instructions.at(j) ) {
        //mDebug() << "Instruction " << j << " => Waypoint " << i;
        d->m_instructionLookupTable.push_back( i );
        ++j;
      }
    }

    reset();
    return true;
}

RoutingModel::Duration RoutingModel::duration() const
{
    return d->m_totalDuration;
}

qreal RoutingModel::totalDistance() const
{
    GeoDataLineString wayPoints;
    foreach( const RouteElement &element, d->m_route ) {
        if ( element.type == WayPoint ) {
            wayPoints << element.position;
        }
    }
    d->m_totalDistance = wayPoints.length( EARTH_RADIUS );

    return d->m_totalDistance;
}

void RoutingModel::exportGpx( QIODevice *device ) const
{
    QString content( "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n" );
    content += "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" creator=\"Marble\" version=\"1.1\" ";
    content += "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ";
    content += "xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 ";
    content += "http://www.topografix.com/GPX/1/1/gpx.xsd\">\n";
    content += "<metadata>\n  <link href=\"http://edu.kde.org/marble\">\n    ";
    content += "<text>Marble Virtual Globe</text>\n  </link>\n</metadata>\n";

    QList<RouteElement> waypoints;
    content += "  <rte>\n    <name>Route</name>\n";
    foreach( const RouteElement &element, d->m_route ) {
        if ( element.type == Instruction ) {
            qreal lon = element.position.longitude( GeoDataCoordinates::Degree );
            qreal lat = element.position.latitude( GeoDataCoordinates::Degree );
            content += QString( "    <rtept lat=\"%1\" lon=\"%2\"><name>%3</name></rtept>\n" ).arg( lat, 0, 'f', 7 ).arg( lon, 0, 'f', 7 ).arg( element.description );
        } else if ( element.type == WayPoint ) {
            waypoints << element;
        }
    }
    content += "  </rte>\n";

    content += "<trk>\n  <name>Route</name>\n    <trkseg>\n";
    foreach( const RouteElement &element, waypoints ) {
        Q_ASSERT( element.type == WayPoint );
        qreal lon = element.position.longitude( GeoDataCoordinates::Degree );
        qreal lat = element.position.latitude( GeoDataCoordinates::Degree );
        content += QString( "      <trkpt lat=\"%1\" lon=\"%2\"></trkpt>\n" ).arg( lat, 0, 'f', 7 ).arg( lon, 0, 'f', 7 );
    }
    content += "    </trkseg>\n  </trk>\n";
    content += "</gpx>\n";

    device->write( content.toUtf8() );
}

void RoutingModel::clear()
{
    d->m_route.clear();
    reset();
}

int RoutingModel::rightNeighbor( const GeoDataCoordinates &position, RouteRequest const *const route ) const
{
    Q_ASSERT( route && "Must not pass a null route ");

    // Quick result for trivial cases
    if ( route->size() < 3 ) {
        return route->size() - 1;
    }

    // Generate an ordered list of all waypoints
    QVector<GeoDataCoordinates> waypoints;
    QMap<int,int> mapping;
    foreach( const RouteElement& element, d->m_route ) {
        if ( element.type == WayPoint ) {
            waypoints << element.position;
        }
    }

    // Force first mapping point to match the route start
    mapping[0] = 0;

    // Calculate the mapping between waypoints and via points
    // Need two for loops to avoid getting stuck in local minima
    for ( int j=1; j<route->size()-1; ++j ) {
        qreal minDistance = -1.0;
        for ( int i=mapping[j-1]; i<waypoints.size(); ++i ) {
            qreal distance = distanceSphere( waypoints[i], route->at(j) );
            if (minDistance < 0.0 || distance < minDistance ) {
                mapping[j] = i;
                minDistance = distance;
            }
        }
    }

    // Determine waypoint with minimum distance to the provided position
    qreal minWaypointDistance = -1.0;
    int waypoint=0;
    for ( int i=0; i<waypoints.size(); ++i ) {
        qreal waypointDistance = distanceSphere( waypoints[i], position );
        if ( minWaypointDistance < 0.0 || waypointDistance < minWaypointDistance ) {
            minWaypointDistance = waypointDistance;
            waypoint = i;
        }
    }

    // Force last mapping point to match the route destination
    mapping[route->size()-1] = waypoints.size()-1;

    // Determine neighbor based on the mapping
    QMap<int, int>::const_iterator iter = mapping.constBegin();
    for ( ; iter != mapping.constEnd(); ++iter ) {
        if ( iter.value() > waypoint ) {
            int index = iter.key();
            Q_ASSERT( index >= 0 && index <= route->size() );
            return index;
        }
    }

    return route->size()-1;
}

void RoutingModel::currentInstruction( GeoDataCoordinates location, qreal speed )
{
    /** @todo FIXME Refactor this method. Shorter, less caching.
              Switch to a more intelligent underlying data structure */

    if( rowCount() != 0 ) {

        QList<RouteElement> instructions;
        QVector<GeoDataCoordinates> wayPoints;
        QVector<GeoDataCoordinates> instructionPoints;

        foreach( const RouteElement &element, d->m_route ) {
            if ( element.type == Instruction ) {
                instructions << element;
                instructionPoints << element.position;
            }

            if( element.type == WayPoint ) {
                wayPoints << element.position;
            }
        }

        qreal minimumWaypointDistance = 0.0;
        int waypointIndex = 0;
        //closest waypoint to the current geolocation
        for( int i = 0; i< wayPoints.size(); ++i ) {
            qreal waypointDistance = distanceSphere( location, wayPoints[i] );
            if( i == 0 ) {
                minimumWaypointDistance = waypointDistance;
                waypointIndex = i;
            }
            else if( waypointDistance < minimumWaypointDistance ) {
                minimumWaypointDistance = waypointDistance;
                waypointIndex = i;
            }
        }

        if ( instructions.isEmpty() ) {
            return;
        }

        d->m_instructionSize = instructions.size();

        qint32 totalTimeRemaining = 0;
        qreal totalDistanceRemaining = 0.0;
        qreal distanceRemaining = 0.0;
        //if there is no route but source and destination are specified
        if( wayPoints.size() != 0 ) {
            if( !( wayPoints[waypointIndex] == wayPoints[wayPoints.size()-1] ) ) {
//                int currentWaypointOffset = 0;
                for ( int k = 0; k<d->m_instructionLookupTable.size(); ++k ) {
                  if ( d->m_instructionLookupTable.at( k ) > waypointIndex ) {
                    d->m_nextInstructionIndex = k;
                    break;
                  }
                }
//                for( int i = 0; i<instructions.size(); ++i ) {
//                    int instructionSize = instructions[i].instructionPointSet.size();
//                    for ( int j = 0; j<instructionSize; ++j ) {
//                       if( wayPoints[waypointIndex] ==  instructions[i].instructionPointSet.at(j) ) {
//                           d->m_nextInstructionIndex = i + 1;
//                           currentWaypointOffset = j;
//                       }
//                    }
//                }

                qreal radius = EARTH_RADIUS;

                if( d->m_nextInstructionIndex != instructions.size() ) {
                    d->m_location = instructions[d->m_nextInstructionIndex].position;
                    d->m_nextDescription = instructions[d->m_nextInstructionIndex].description;
                    d->m_nextInstructionPixmap = d->m_turnTypePixmaps[instructions[d->m_nextInstructionIndex].turnType];
                    if ( d->m_nextInstructionIndex+1 < instructions.size() ) {
                        d->m_followingInstructionPixmap = d->m_turnTypePixmaps[instructions[d->m_nextInstructionIndex+1].turnType];
                    } else if ( d->m_request->size() > 0 ) {
                        d->m_followingInstructionPixmap = d->m_request->pixmap( d->m_request->size()-1 );
                    }
                    //distance between current position and next instruction point
                    distanceRemaining = distanceSphere( location, d->m_location ) * radius;
                }
                else {
                    GeoDataCoordinates destinationCoord =  instructions[d->m_nextInstructionIndex-1].instructionPointSet.last();
                    distanceRemaining = distanceSphere( location, destinationCoord ) * radius;
//                    if( !(instructions[d->m_nextInstructionIndex-1].instructionPointSet.at( currentWaypointOffset ) == destinationCoord ) ) {
//                        distanceRemaining = distanceSphere( location, destinationCoord ) * radius;
//                    }
//                    else {
//                        distanceRemaining = 0.0;
//                    }
                }

                d->m_currentInstructionLength = instructions[d->m_nextInstructionIndex-1].instructionDistance;

                GeoDataLineString remainingInstructionPoints;
                for( int i = waypointIndex; i < wayPoints.size(); ++i ) {
                    remainingInstructionPoints << wayPoints[i];
                }

                totalDistanceRemaining = remainingInstructionPoints.length( radius );
                if( speed != 0 ) {
                    d->m_timeRemaining = ( distanceRemaining / speed ) * SEC2MIN;
                    totalTimeRemaining = qint32( totalDistanceRemaining / speed );
                }
            }
            else {
                totalTimeRemaining = 0.0;
                totalDistanceRemaining = 0.0;
            }
        }
        d->m_nextInstructionDistance = distanceRemaining;
        d->m_totalTimeRemaining = totalTimeRemaining;
        d->m_totalDistanceRemaining = totalDistanceRemaining;

        emit nextInstruction( d->m_totalTimeRemaining, d->m_totalDistanceRemaining );

        bool const deviated = d->deviatedFromRoute( location, wayPoints );
        RoutingModelPrivate::RouteDeviation const deviation = deviated ? RoutingModelPrivate::OffRoute : RoutingModelPrivate::OnRoute;
        if ( d->m_deviation != deviation ) {
            d->m_deviation = deviation;
            emit deviatedFromRoute( deviated );
        }

        d->updateViaPoints( location );
    }
}

qreal RoutingModel::remainingTime() const
{
   return d->m_timeRemaining;
}

qint32 RoutingModel::totalTimeRemaining() const
{
    return d->m_totalTimeRemaining;
}

GeoDataCoordinates RoutingModel::instructionPoint() const
{
    return d->m_location;
}

QString RoutingModel::instructionText() const
{
    if( d->m_nextInstructionIndex < d->m_instructionSize ) {
        return d->m_nextDescription;
    }
    else {
        return QString::null;
    }
}

bool RoutingModel::deviatedFromRoute() const
{
    return d->m_deviation != RoutingModelPrivate::OnRoute;
}

qreal RoutingModel::nextInstructionDistance() const
{
    return d->m_nextInstructionDistance;
}

qreal RoutingModel::currentInstructionLength() const
{
    return d->m_currentInstructionLength;
}

QPixmap RoutingModel::nextInstructionPixmap() const
{
    return d->m_nextInstructionPixmap;
}

QPixmap RoutingModel::followingInstructionPixmap() const
{
    return d->m_followingInstructionPixmap;
}

} // namespace Marble

#include "RoutingModel.moc"
