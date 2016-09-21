//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "MonavRunner.h"
#include "MonavPlugin.h"
#include "signals.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "routing/RouteRequest.h"
#include "routing/instructions/InstructionTransformation.h"
#include "GeoDataDocument.h"
#include "GeoDataData.h"
#include "GeoDataExtendedData.h"
#include "GeoDataPlacemark.h"
#include "GeoDataLineString.h"

#include <QTime>
#include <QLocalSocket>

using namespace MoNav;

namespace Marble
{

class MonavRunnerPrivate
{
public:
    const MonavPlugin* m_plugin;

    MonavRunnerPrivate( const MonavPlugin* plugin );

    bool retrieveData( const RouteRequest *route, RoutingResult* result ) const;

    bool retrieveData( const RouteRequest *route, const QString &mapDir, RoutingResult* result ) const;

    int retrieveRoute( const RouteRequest *route, QVector<GeoDataPlacemark*> *instructions, GeoDataLineString* geometry ) const;

    static GeoDataDocument* createDocument( GeoDataLineString *geometry, const QVector<GeoDataPlacemark*> &instructions, const QString &name, const GeoDataExtendedData &data );
};

MonavRunnerPrivate::MonavRunnerPrivate( const MonavPlugin* plugin ) :
        m_plugin( plugin )
{
    // nothing to do
}

bool MonavRunnerPrivate::retrieveData( const RouteRequest *route, RoutingResult* reply ) const
{
    QString mapDir = m_plugin->mapDirectoryForRequest( route );
    if ( mapDir.isEmpty() ) {
        return false;
    }

    if ( retrieveData( route, mapDir, reply ) ) {
        return true;
    }

    // The separation into two different methods to determine a first country candidate
    // and a list of alternative ones if the first candidate fails is intentional
    // for performance reasons. Do not merge both.
    QStringList alternatives = m_plugin->mapDirectoriesForRequest( route );
    alternatives.removeOne( mapDir );
    foreach( const QString &mapDir, alternatives ) {
        if ( retrieveData( route, mapDir, reply ) ) {
            return true;
        }
    }

    return false;
}

bool MonavRunnerPrivate::retrieveData( const RouteRequest *route, const QString &mapDir, RoutingResult* reply ) const
{
    QLocalSocket socket;
    socket.connectToServer( "MoNavD" );
    if ( socket.waitForConnected() ) {
        if ( m_plugin->monavVersion() == MonavPlugin::Monav_0_3 ) {
            CommandType commandType;
            commandType.value = CommandType::RoutingCommand;
            commandType.post( &socket );
        }

        RoutingCommand command;
        QVector<Node> waypoints;

        for ( int i = 0; i < route->size(); ++i ) {
            Node coordinate;
            coordinate.longitude = route->at( i ).longitude( GeoDataCoordinates::Degree );
            coordinate.latitude = route->at( i ).latitude( GeoDataCoordinates::Degree );
            waypoints << coordinate;
        }

        command.dataDirectory = mapDir;
        command.lookupRadius = 1500;
        command.waypoints = waypoints;
        command.lookupStrings = true;

        command.post( &socket );
        socket.flush();

        if ( reply->read( &socket ) ) {
            switch ( reply->type ) {
            case RoutingResult::LoadFailed:
                mDebug() << "failed to load monav map from " << mapDir;
                return false;
                break;
            case RoutingResult::RouteFailed:
                mDebug() << "failed to retrieve route from monav daemon";
                return false;
                break;
            case RoutingResult::TypeLookupFailed:
                mDebug() << "failed to lookup type from monav daemon";
                return false;
                break;
            case RoutingResult::NameLookupFailed:
                mDebug() << "failed to lookup name from monav daemon";
                return false;
                break;
            case RoutingResult::Success:
                return true;
            }
        } else {
            mDebug() << "Failed to read reply";
        }
    } else {
        mDebug() << "No connection to MoNavD";
    }

    return false;
}

int MonavRunnerPrivate::retrieveRoute( const Marble::RouteRequest* route, QVector< Marble::GeoDataPlacemark* >* instructions, Marble::GeoDataLineString* geometry ) const
{
    RoutingResult reply;
    if ( retrieveData( route, &reply ) ) {
        /** @todo: make use of reply.seconds, the estimated travel time */
        for ( int i = 0; i < reply.pathNodes.size(); ++i ) {
            qreal lon = reply.pathNodes[i].longitude;
            qreal lat = reply.pathNodes[i].latitude;
            GeoDataCoordinates coordinates( lon, lat, 0, GeoDataCoordinates::Degree );
            geometry->append( coordinates );
        }

        RoutingWaypoints waypoints;
        int k = 0;
        for ( int i = 0; i < reply.pathEdges.size(); ++i ) {
            QString road = reply.nameStrings[reply.pathEdges[i].name];
            QString type = reply.typeStrings[reply.pathEdges[i].type];
            RoutingWaypoint::JunctionType junction = RoutingWaypoint::Other;
            if (type == QLatin1String("roundabout") && reply.pathEdges[i].branchingPossible) {
                junction = RoutingWaypoint::Roundabout;
            }
            for ( unsigned int l = 0; l < reply.pathEdges[i].length; ++k, ++l ) {
                qreal lon = reply.pathNodes[k].longitude;
                qreal lat = reply.pathNodes[k].latitude;
                RoutingPoint point( lon, lat );
                bool const last = l == reply.pathEdges[i].length - 1;
                RoutingWaypoint::JunctionType finalJunction = last ? junction : ( reply.pathEdges[i].branchingPossible ? RoutingWaypoint::Other : RoutingWaypoint::None );
                RoutingWaypoint waypoint( point, finalJunction, "", type, -1, road );
                waypoints.push_back( waypoint );
            }
        }

        RoutingInstructions directions = InstructionTransformation::process( waypoints );
        for ( int i = 0; i < directions.size(); ++i ) {
            GeoDataPlacemark* placemark = new GeoDataPlacemark( directions[i].instructionText() );
            GeoDataExtendedData extendedData;
            GeoDataData turnType;
            turnType.setName(QStringLiteral("turnType"));
            turnType.setValue( qVariantFromValue<int>( int( directions[i].turnType() ) ) );
            extendedData.addValue( turnType );
            GeoDataData roadName;
            roadName.setName(QStringLiteral("roadName"));
            roadName.setValue( directions[i].roadName() );
            extendedData.addValue( roadName );
            placemark->setExtendedData( extendedData );
            Q_ASSERT( !directions[i].points().isEmpty() );
            GeoDataLineString* geometry = new GeoDataLineString;
            QVector<RoutingWaypoint> items = directions[i].points();
            for ( int j = 0; j < items.size(); ++j ) {
                RoutingPoint point = items[j].point();
                GeoDataCoordinates coordinates( point.lon(), point.lat(), 0.0, GeoDataCoordinates::Degree );
                geometry->append( coordinates );
            }
            placemark->setGeometry( geometry );
            instructions->push_back( placemark );
        }
        int duration = (int) reply.seconds;
        return duration;
    }
    return 0;
}

GeoDataDocument* MonavRunnerPrivate::createDocument( Marble::GeoDataLineString* geometry, const QVector< Marble::GeoDataPlacemark* >& instructions, const QString& name, const Marble::GeoDataExtendedData& data )
{
    if ( !geometry || geometry->isEmpty() ) {
        return 0;
    }

    GeoDataDocument* result = new GeoDataDocument;
    GeoDataPlacemark* routePlacemark = new GeoDataPlacemark;
    routePlacemark->setName(QStringLiteral("Route"));
    routePlacemark->setGeometry( geometry );
    routePlacemark->setExtendedData( data );
    result->append( routePlacemark );

    foreach( GeoDataPlacemark* placemark, instructions ) {
        result->append( placemark );
    }

    result->setName( name );
    return result;
}

MonavRunner::MonavRunner( const MonavPlugin* plugin, QObject *parent ) :
        RoutingRunner( parent ),
        d( new MonavRunnerPrivate( plugin ) )
{
    // nothing to do
}

MonavRunner::~MonavRunner()
{
    delete d;
}

void MonavRunner::retrieveRoute( const RouteRequest *route )
{
    QVector<GeoDataPlacemark*> instructions;
    QTime time;
    GeoDataLineString* waypoints = new GeoDataLineString();
    int duration = d->retrieveRoute( route, &instructions, waypoints );
    time = time.addSecs( duration );
    qreal length = waypoints->length( EARTH_RADIUS );
    const QString name = nameString( "Monav", length, time );
    const GeoDataExtendedData data = routeData( length, time );
    GeoDataDocument *result = d->createDocument( waypoints, instructions, name, data );
    emit routeCalculated( result );
}

#if 0
void MonavRunner::reverseGeocoding( const GeoDataCoordinates &coordinates )
{
    GeoDataPlacemark placemark;
    placemark.setCoordinate( GeoDataPoint( coordinates ) );

    RouteRequest route;
    route.append( coordinates );
    route.append( coordinates );
    RoutingResult reply;

    if ( d->retrieveData( &route, &reply ) && !reply.pathEdges.isEmpty() ) {
        QString road = reply.nameStrings[reply.pathEdges[0].name];
        placemark.setAddress( road );
        GeoDataExtendedData extendedData;
        extendedData.addValue( GeoDataData( "road", road ) );
        placemark.setExtendedData( extendedData );
    }

    emit reverseGeocodingFinished( coordinates, placemark );
}
#endif

} // namespace Marble

#include "moc_MonavRunner.cpp"
