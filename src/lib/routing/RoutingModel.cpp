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

    Route m_route;
    RouteSegment m_segment;

    RouteDeviation m_deviation;
    QMap<RoutingInstruction::TurnType,QString> m_turnTypePixmaps;
    PositionTracking* m_positionTracking;
    RouteRequest* m_request;
    GeoDataCoordinates m_position;

    void importPlacemark( RouteSegment &outline, QVector<RouteSegment> &segments, const GeoDataPlacemark *placemark );

    bool deviatedFromRoute( const GeoDataCoordinates &position, const QVector<GeoDataCoordinates> &waypoints ) const;

    void updateViaPoints( const GeoDataCoordinates &position );
};

RoutingModelPrivate::RoutingModelPrivate( RouteRequest* request )
    : m_deviation( Unknown ),
      m_positionTracking( 0 ),
      m_request( request )
{
    m_turnTypePixmaps[RoutingInstruction::Unknown] = MarbleDirs::path( "bitmaps/routing_step.png" );
    m_turnTypePixmaps[RoutingInstruction::Straight] = ":/data/bitmaps/turn-continue.png";
    m_turnTypePixmaps[RoutingInstruction::SlightRight] = ":/data/bitmaps/turn-slight-right.png";
    m_turnTypePixmaps[RoutingInstruction::Right] = ":/data/bitmaps/turn-right.png";
    m_turnTypePixmaps[RoutingInstruction::SharpRight] = ":/data/bitmaps/turn-sharp-right.png";
    m_turnTypePixmaps[RoutingInstruction::TurnAround] = ":/data/bitmaps/turn-around.png";
    m_turnTypePixmaps[RoutingInstruction::SharpLeft] = ":/data/bitmaps/turn-sharp-left.png";
    m_turnTypePixmaps[RoutingInstruction::Left] = ":/data/bitmaps/turn-left.png";
    m_turnTypePixmaps[RoutingInstruction::SlightLeft] = ":/data/bitmaps/turn-slight-left.png";
    m_turnTypePixmaps[RoutingInstruction::RoundaboutFirstExit] = ":/data/bitmaps/turn-roundabout-first.png";
    m_turnTypePixmaps[RoutingInstruction::RoundaboutSecondExit] = ":/data/bitmaps/turn-roundabout-second.png";
    m_turnTypePixmaps[RoutingInstruction::RoundaboutThirdExit] = ":/data/bitmaps/turn-roundabout-third.png";
    m_turnTypePixmaps[RoutingInstruction::RoundaboutExit] = ":/data/bitmaps/turn-roundabout-far.png";
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

void RoutingModelPrivate::importPlacemark( RouteSegment &outline, QVector<RouteSegment> &segments, const GeoDataPlacemark *placemark )
{
    GeoDataGeometry* geometry = placemark->geometry();
    GeoDataLineString* lineString = dynamic_cast<GeoDataLineString*>( geometry );
    QStringList blacklist = QStringList() << "" << "Route" << "Tessellated";
    RouteSegment segment;
    bool isOutline = true;
    if ( !blacklist.contains( placemark->name() ) ) {
        if( lineString ) {
            Maneuver maneuver;
            maneuver.setInstructionText( placemark->name() );
            maneuver.setPosition( lineString->at( 0 ) );

            if ( placemark->extendedData().contains( "turnType" ) ) {
                QVariant turnType = placemark->extendedData().value( "turnType" ).value();
                // The enum value is converted to/from an int in the QVariant
                // because only a limited set of data types can be serialized with QVariant's
                // toString() method (which is used to serialize <ExtendedData>/<Data> values)
                maneuver.setDirection( RoutingInstruction::TurnType( qVariantValue<int>( turnType ) ) );
            }

            segment.setManeuver( maneuver );
            isOutline = false;
        }
    }

    if ( lineString ) {
        segment.setPath( *lineString );

        if ( isOutline ) {
            outline = segment;
        } else {
            segments.push_back( segment );
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
    return parent.isValid() ? 0 : d->m_route.turnPoints().size();
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

    if ( index.row() < d->m_route.turnPoints().size() && index.column() == 0 ) {
        RouteSegment segment = d->m_route.firstRouteSegment();
        for ( int i=0; i<index.row(); ++i ) {
            segment = segment.nextRouteSegment();
        }
        switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return segment.maneuver().instructionText();
            break;
        case Qt::DecorationRole:
            {
                bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
                if ( smallScreen ) {
                    return QPixmap( d->m_turnTypePixmaps[segment.maneuver().direction()] );
                } else {
                    return QPixmap( d->m_turnTypePixmaps[segment.maneuver().direction()] ).scaled( 32, 32 );
                }

                return QVariant();
            }
            break;
        case CoordinateRole:
            return QVariant::fromValue( segment.maneuver().position() );
            break;
        default:
            return QVariant();
        }
    }

    return QVariant();
}

bool RoutingModel::setCurrentRoute( GeoDataDocument* document )
{
    d->m_route = Route();
    d->m_segment = RouteSegment();
    QVector<RouteSegment> segments;
    RouteSegment outline;

    QVector<GeoDataFolder*> folders = document->folderList();
    foreach( const GeoDataFolder *folder, folders ) {
        foreach( const GeoDataPlacemark *placemark, folder->placemarkList() ) {
            d->importPlacemark( outline, segments, placemark );
        }
    }

    foreach( const GeoDataPlacemark *placemark, document->placemarkList() ) {
        d->importPlacemark( outline, segments, placemark );
    }

    if ( segments.isEmpty() ) {
        segments << outline;
    }

    if ( segments.size() > 0 ) {
        for ( int i=segments.size()-2; i>=0; --i ) {
            segments[i].setNextRouteSegment( segments[i+1] );
        }
        d->m_segment = segments[0];
        d->m_route.setFirstRouteSegment( d->m_segment );
    }

    d->m_deviation = RoutingModelPrivate::Unknown;

    reset();
    emit currentRouteChanged();
    return true;
}

RoutingModel::Duration RoutingModel::duration() const
{
    Duration duration;
    QTime time;
    time.addSecs( d->m_route.travelTime() );
    duration.time = time;
    return duration;
}

qreal RoutingModel::totalDistance() const
{
    return d->m_route.distance();
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

    content += "  <rte>\n    <name>Route</name>\n";
    RouteSegment segment = d->m_route.firstRouteSegment();
    while ( segment.isValid() ) {
        qreal lon = segment.maneuver().position().longitude( GeoDataCoordinates::Degree );
        qreal lat = segment.maneuver().position().latitude( GeoDataCoordinates::Degree );
        QString const text = segment.maneuver().instructionText();
        content += QString( "    <rtept lat=\"%1\" lon=\"%2\"><name>%3</name></rtept>\n" ).arg( lat, 0, 'f', 7 ).arg( lon, 0, 'f', 7 ).arg( text );
    }
    content += "  </rte>\n";

    content += "<trk>\n  <name>Route</name>\n    <trkseg>\n";
    GeoDataLineString points = d->m_route.path();
    for ( int i=0; i<points.size(); ++i ) {
        qreal lon = points[i].longitude( GeoDataCoordinates::Degree );
        qreal lat = points[i].latitude( GeoDataCoordinates::Degree );
        content += QString( "      <trkpt lat=\"%1\" lon=\"%2\"></trkpt>\n" ).arg( lat, 0, 'f', 7 ).arg( lon, 0, 'f', 7 );
    }
    content += "    </trkseg>\n  </trk>\n";
    content += "</gpx>\n";

    device->write( content.toUtf8() );
}

void RoutingModel::clear()
{
    d->m_route = Route();
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
    GeoDataLineString points = d->m_route.path();
    QMap<int,int> mapping;

    // Force first mapping point to match the route start
    mapping[0] = 0;

    // Calculate the mapping between waypoints and via points
    // Need two for loops to avoid getting stuck in local minima
    for ( int j=1; j<route->size()-1; ++j ) {
        qreal minDistance = -1.0;
        for ( int i=mapping[j-1]; i<points.size(); ++i ) {
            qreal distance = distanceSphere( points[i], route->at(j) );
            if (minDistance < 0.0 || distance < minDistance ) {
                mapping[j] = i;
                minDistance = distance;
            }
        }
    }

    // Determine waypoint with minimum distance to the provided position
    qreal minWaypointDistance = -1.0;
    int waypoint=0;
    for ( int i=0; i<points.size(); ++i ) {
        qreal waypointDistance = distanceSphere( points[i], position );
        if ( minWaypointDistance < 0.0 || waypointDistance < minWaypointDistance ) {
            minWaypointDistance = waypointDistance;
            waypoint = i;
        }
    }

    // Force last mapping point to match the route destination
    mapping[route->size()-1] = points.size()-1;

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

void RoutingModel::currentInstruction( GeoDataCoordinates location, qreal /*speed*/ )
{
    d->m_position = location;
    qreal distance(0.0);
    d->m_segment = d->m_route.closestSegmentTo( d->m_position, distance );

    d->updateViaPoints( d->m_position );

    /** @todo: use correct values */
    emit nextInstruction( 0.0, d->m_segment.distance() );

    qreal deviation = 0.0;
    if ( d->m_positionTracking && d->m_positionTracking->accuracy().vertical > 0.0 ) {
        deviation = qMax<qreal>( d->m_positionTracking->accuracy().vertical, d->m_positionTracking->accuracy().horizontal );
    }
    qreal const threshold = deviation + 100.0;

    RoutingModelPrivate::RouteDeviation const deviated = distance < threshold ? RoutingModelPrivate::OnRoute : RoutingModelPrivate::OffRoute;
    if ( d->m_deviation != deviated ) {
        d->m_deviation = deviated;
        emit deviatedFromRoute( deviated == RoutingModelPrivate::OffRoute);
    }
}

qreal RoutingModel::remainingTime() const
{
    /** @todo: more precise, accumulate? */
   return d->m_segment.travelTime();
}

qint32 RoutingModel::totalTimeRemaining() const
{
    /** @todo: more precise, accumulate? */
    return d->m_segment.travelTime();
}

GeoDataCoordinates RoutingModel::instructionPoint() const
{
    return d->m_segment.nextRouteSegment().maneuver().position();
}

QString RoutingModel::instructionText() const
{
    return d->m_segment.nextRouteSegment().maneuver().instructionText();
}

bool RoutingModel::deviatedFromRoute() const
{
    return d->m_deviation != RoutingModelPrivate::OnRoute;
}

qreal RoutingModel::nextInstructionDistance() const
{
    /** @todo: implement correctly */
    return EARTH_RADIUS * distanceSphere( d->m_position, d->m_segment.maneuver().position() );
}

qreal RoutingModel::currentInstructionLength() const
{
    /** @todo: remove, unused */
    return d->m_segment.distance();
}

QString RoutingModel::nextInstructionPixmapFile() const
{
    return d->m_turnTypePixmaps[d->m_segment.nextRouteSegment().maneuver().direction()];
}

QPixmap RoutingModel::nextInstructionPixmap() const
{
    return QPixmap( nextInstructionPixmapFile() );
}

RoutingInstruction::TurnType RoutingModel::nextTurnType() const
{
    return d->m_segment.nextRouteSegment().nextRouteSegment().maneuver().direction();
}

QPixmap RoutingModel::followingInstructionPixmap() const
{
    return QPixmap( d->m_turnTypePixmaps[d->m_segment.nextRouteSegment().nextRouteSegment().maneuver().direction()] );
}

int RoutingModel::nextTurnIndex() const
{
    /** @todo: implement correctly */
    return 42;
}

const Route & RoutingModel::route() const
{
    return d->m_route;
}

} // namespace Marble

#include "RoutingModel.moc"
