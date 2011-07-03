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
#include <QtCore/QHash>
#include <QtCore/QByteArray>
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

    RouteDeviation m_deviation;
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
    // nothing to do
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
                 this, SLOT( updatePosition( GeoDataCoordinates, qreal ) ) );
    }

   QHash<int, QByteArray> roles = roleNames();
   roles.insert( RoutingModel::TurnTypeIconRole, "turnTypeIcon" );
   setRoleNames( roles );
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
                bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
                if ( smallScreen ) {
                    return QPixmap( segment.maneuver().directionPixmap() );
                } else {
                    return QPixmap( segment.maneuver().directionPixmap() ).scaled( 32, 32 );
                }

                return QVariant();
            }
            break;
        case RoutingModel::CoordinateRole:
            return QVariant::fromValue( segment.maneuver().position() );
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

bool RoutingModel::setCurrentRoute( GeoDataDocument* document )
{
    d->m_route = Route();
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
        foreach( const RouteSegment &segment, segments ) {
            d->m_route.addRouteSegment( segment );
        }
    }

    d->m_deviation = RoutingModelPrivate::Unknown;

    reset();
    emit currentRouteChanged();
    return true;
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
    for ( int i=0; i<d->m_route.size(); ++i ) {
        const RouteSegment &segment = d->m_route.at( i );
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

void RoutingModel::updatePosition( GeoDataCoordinates location, qreal /*speed*/ )
{
    d->m_position = location;
    d->m_route.setPosition( location );

    d->updateViaPoints( d->m_position );
    qreal distance = EARTH_RADIUS * distanceSphere( location, d->m_route.positionOnRoute() );
    emit positionChanged();

    qreal deviation = 0.0;
    if ( d->m_positionTracking && d->m_positionTracking->accuracy().vertical > 0.0 ) {
        deviation = qMax<qreal>( d->m_positionTracking->accuracy().vertical, d->m_positionTracking->accuracy().horizontal );
    }
    qreal const threshold = deviation + 100.0;

    RoutingModelPrivate::RouteDeviation const deviated = distance < threshold ? RoutingModelPrivate::OnRoute : RoutingModelPrivate::OffRoute;
    if ( d->m_deviation != deviated ) {
        d->m_deviation = deviated;
        emit deviatedFromRoute( deviated == RoutingModelPrivate::OffRoute );
    }
}

bool RoutingModel::deviatedFromRoute() const
{
    return d->m_deviation != RoutingModelPrivate::OnRoute;
}

const Route & RoutingModel::route() const
{
    return d->m_route;
}

} // namespace Marble

#include "RoutingModel.moc"
