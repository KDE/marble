//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "RoutingModel.h"

#include "Planet.h"
#include "MarbleMath.h"
#include "Route.h"
#include "RouteRequest.h"
#include "PositionTracking.h"
#include "MarbleModel.h"
#include "MarbleGlobal.h"
#include "GeoDataAccuracy.h"

#include <QPixmap>

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

    explicit RoutingModelPrivate( RouteRequest* request );
    MarbleModel *m_marbleModel;

    Route m_route;

    RouteDeviation m_deviation;
    PositionTracking* m_positionTracking;
    RouteRequest* const m_request;
    QHash<int, QByteArray> m_roleNames;

    void updateViaPoints( const GeoDataCoordinates &position );
};

RoutingModelPrivate::RoutingModelPrivate( RouteRequest* request )
    : m_deviation( Unknown ),
      m_positionTracking( 0 ),
      m_request( request )
{
    // nothing to do
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

RoutingModel::RoutingModel( RouteRequest* request, MarbleModel *model, QObject *parent ) :
        QAbstractListModel( parent ), d( new RoutingModelPrivate( request ) )
{
    d->m_marbleModel = model;
    d->m_positionTracking = model->positionTracking();
    QObject::connect( d->m_positionTracking, SIGNAL(gpsLocation(GeoDataCoordinates,qreal)),
             this, SLOT(updatePosition(GeoDataCoordinates,qreal)) );

   QHash<int, QByteArray> roles;
   roles.insert( Qt::DisplayRole, "display" );
   roles.insert( RoutingModel::TurnTypeIconRole, "turnTypeIcon" );
   roles.insert( RoutingModel::LongitudeRole, "longitude" );
   roles.insert( RoutingModel::LatitudeRole, "latitude" );
   d->m_roleNames = roles;
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
        const RouteSegment &segment = d->m_route.at( index.row() );
        switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return segment.maneuver().instructionText();
            break;
        case Qt::DecorationRole:
            {
                bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
                if ( segment.maneuver().hasWaypoint() ) {
                    int const size = smallScreen ? 64 : 32;
                    return d->m_request->pixmap( segment.maneuver().waypointIndex(), size, size/4 );
                } else {
                    QPixmap const pixmap = segment.maneuver().directionPixmap();
                    return smallScreen ? pixmap : pixmap.scaled( 32, 32 );
                }
            }
            break;
        case Qt::SizeHintRole:
            {
                bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
                int const size = smallScreen ? 64 : 32;
                return QSize( size, size );
            }
            break;
        case RoutingModel::CoordinateRole:
            return QVariant::fromValue( segment.maneuver().position() );
            break;
        case RoutingModel::LongitudeRole:
            return QVariant(segment.maneuver().position().longitude(GeoDataCoordinates::Degree));
            break;
        case RoutingModel::LatitudeRole:
            return QVariant(segment.maneuver().position().latitude(GeoDataCoordinates::Degree));
            break;
        case RoutingModel::TurnTypeIconRole:
            return segment.maneuver().directionPixmap();
            break;
        default:
            return QVariant();
        }
    }

    return QVariant();
}

QHash<int, QByteArray> RoutingModel::roleNames() const
{
    return d->m_roleNames;
}

void RoutingModel::setRoute( const Route &route )
{
    d->m_route = route;
    d->m_deviation = RoutingModelPrivate::Unknown;

    beginResetModel();
    endResetModel();
    emit currentRouteChanged();
}

void RoutingModel::exportGpx( QIODevice *device ) const
{
    QString content = QLatin1String("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n"
        "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" creator=\"Marble\" version=\"1.1\" "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
        "xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 "
        "http://www.topografix.com/GPX/1/1/gpx.xsd\">\n"
        "<metadata>\n  <link href=\"http://edu.kde.org/marble\">\n    "
        "<text>Marble Virtual Globe</text>\n  </link>\n</metadata>\n"
        "  <rte>\n    <name>Route</name>\n");
    bool hasAltitude = false;
    for ( int i=0; !hasAltitude && i<d->m_route.size(); ++i ) {
        hasAltitude = d->m_route.at( i ).maneuver().position().altitude() != 0.0;
    }
    for ( int i=0; i<d->m_route.size(); ++i ) {
        const Maneuver &maneuver = d->m_route.at( i ).maneuver();
        qreal lon = maneuver.position().longitude( GeoDataCoordinates::Degree );
        qreal lat = maneuver.position().latitude( GeoDataCoordinates::Degree );
        QString const text = maneuver.instructionText();
        content += QString( "    <rtept lat=\"%1\" lon=\"%2\">\n" ).arg( lat, 0, 'f', 7 ).arg( lon, 0, 'f', 7 );
        content += QString( "        <name>%1</name>\n").arg( text );
        if ( hasAltitude ) {
            content += QString( "        <ele>%1</ele>\n" ).arg( maneuver.position().altitude(), 0, 'f', 2 );
        }
        content += QString( "    </rtept>\n" );
    }
    content += QLatin1String("  </rte>\n"
        "<trk>\n  <name>Route</name>\n    <trkseg>\n");
    GeoDataLineString points = d->m_route.path();
    hasAltitude = false;
    for ( int i=0; !hasAltitude && i<points.size(); ++i ) {
        hasAltitude = points[i].altitude() != 0.0;
    }
    for ( int i=0; i<points.size(); ++i ) {
        GeoDataCoordinates const &point = points[i];
        qreal lon = point.longitude( GeoDataCoordinates::Degree );
        qreal lat = point.latitude( GeoDataCoordinates::Degree );
        content += QString( "      <trkpt lat=\"%1\" lon=\"%2\">\n" ).arg( lat, 0, 'f', 7 ).arg( lon, 0, 'f', 7 );
        if ( hasAltitude ) {
            content += QString( "        <ele>%1</ele>\n" ).arg( point.altitude(), 0, 'f', 2 );
        }
        content += QString( "      </trkpt>\n" );
    }
    content += QLatin1String("    </trkseg>\n  </trk>\n"
        "</gpx>\n");

    device->write( content.toUtf8() );
}

void RoutingModel::clear()
{
    d->m_route = Route();
    beginResetModel();
    endResetModel();
    emit currentRouteChanged();
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

void RoutingModel::updatePosition( GeoDataCoordinates location, qreal speed )
{
    d->m_route.setPosition( location );

    d->updateViaPoints( location );
    qreal planetRadius = d->m_marbleModel->planet()->radius();
    qreal distance = planetRadius * distanceSphere( location, d->m_route.positionOnRoute() );
    emit positionChanged();

    qreal deviation = 0.0;
    if ( d->m_positionTracking && d->m_positionTracking->accuracy().vertical > 0.0 ) {
        deviation = qMax<qreal>( d->m_positionTracking->accuracy().vertical, d->m_positionTracking->accuracy().horizontal );
    }
    qreal const threshold = deviation + qBound(10.0, speed*10.0, 150.0);

    RoutingModelPrivate::RouteDeviation const deviated = distance < threshold ? RoutingModelPrivate::OnRoute : RoutingModelPrivate::OffRoute;
    if ( d->m_deviation != deviated ) {
        d->m_deviation = deviated;
        emit deviatedFromRoute( deviated == RoutingModelPrivate::OffRoute );
    }
}

bool RoutingModel::deviatedFromRoute() const
{
    return d->m_deviation == RoutingModelPrivate::OffRoute;
}

const Route & RoutingModel::route() const
{
    return d->m_route;
}

} // namespace Marble

#include "moc_RoutingModel.cpp"
