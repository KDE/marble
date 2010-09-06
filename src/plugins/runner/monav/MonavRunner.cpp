//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MonavRunner.h"
#include "signals.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "routing/RouteRequest.h"
#include "GeoDataDocument.h"

#include <QtCore/QProcess>
#include <QtCore/QTime>
#include <QtNetwork/QLocalSocket>

namespace Marble
{

class MonavRunnerPrivate
{
public:
    QDir m_mapDir;

    MonavRunnerPrivate();

    GeoDataLineString* retrieveRoute( RouteRequest *route ) const;

    GeoDataDocument* createDocument( GeoDataLineString* geometry ) const;
};

MonavRunnerPrivate::MonavRunnerPrivate() :
        m_mapDir( MarbleDirs::localPath() + "/maps/earth/monav/" )
{
    // nothing to do
}

GeoDataLineString* MonavRunnerPrivate::retrieveRoute( RouteRequest *route ) const
{
    GeoDataLineString* geometry = new GeoDataLineString;

    QLocalSocket socket;
    socket.connectToServer( "MoNavD" );
    if ( socket.waitForConnected() ) {
        RoutingDaemonCommand command;
        QVector<RoutingDaemonCoordinate> waypoints;

        for ( int i = 0; i < route->size(); ++i )
        {
            RoutingDaemonCoordinate coordinate;
            coordinate.longitude = route->at( i ).longitude( GeoDataCoordinates::Degree );
            coordinate.latitude = route->at( i ).latitude( GeoDataCoordinates::Degree );
            waypoints << coordinate;
        }

        command.dataDirectory = m_mapDir.absolutePath();
        command.lookupRadius = 1500;
        command.waypoints = waypoints;

        command.post( &socket );
        socket.flush();

        RoutingDaemonResult reply;
        if ( reply.read( &socket ) )
        {
            switch (reply.type)
            {
            case RoutingDaemonResult::LoadFail:
                mDebug() << "failed to load monav map from " << m_mapDir.absolutePath();
                break;
            case RoutingDaemonResult::RouteFail:
                mDebug() << "failed to retrieve route from monav daemon";
                break;
            case RoutingDaemonResult::Success:
                /** @todo: make use of reply.seconds, the estimated travel time */
                for ( int i = 0; i < reply.path.size(); i++ ) {
                    qreal lon = reply.path[i].longitude;
                    qreal lat = reply.path[i].latitude;
                    GeoDataCoordinates coordinates( lon, lat, 0, GeoDataCoordinates::Degree );
                    geometry->append( coordinates );
                }
                break;
            }
        }
    }

    return geometry;
}

GeoDataDocument* MonavRunnerPrivate::createDocument( GeoDataLineString *geometry ) const
{
    if ( !geometry || geometry->isEmpty() ) {
        return 0;
    }

    GeoDataDocument* result = new GeoDataDocument;
    GeoDataPlacemark* routePlacemark = new GeoDataPlacemark;
    routePlacemark->setName( "Route" );
    routePlacemark->setGeometry( geometry );
    result->append( routePlacemark );

    QString name = "%1 %2 (Monav)";
    QString unit = "m";
    qreal length = geometry->length( EARTH_RADIUS );
    if ( length >= 1000 )
    {
        length /= 1000.0;
        unit = "km";
    }
    result->setName( name.arg( length, 0, 'f', 1 ).arg( unit ) );
    return result;
}

MonavRunner::MonavRunner( QObject *parent ) :
        MarbleAbstractRunner( parent ),
        d( new MonavRunnerPrivate )
{
    // nothing to do
}

MonavRunner::~MonavRunner()
{
    delete d;
}

GeoDataFeature::GeoDataVisualCategory MonavRunner::category() const
{
    return GeoDataFeature::OsmSite;
}

void MonavRunner::retrieveRoute( RouteRequest *route )
{
    GeoDataLineString* waypoints = d->retrieveRoute( route );
    GeoDataDocument* result = d->createDocument( waypoints );
    emit routeCalculated( result );
}

} // namespace Marble

#include "MonavRunner.moc"
