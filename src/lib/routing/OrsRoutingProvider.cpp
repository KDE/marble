//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "OrsRoutingProvider.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleLocale.h"
#include "RouteSkeleton.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

namespace Marble
{

OrsRoutingProvider::OrsRoutingProvider( QObject *parent ) :
        AbstractRoutingProvider( parent ),
        m_networkAccessManager( new QNetworkAccessManager( this ) )
{
    connect( m_networkAccessManager, SIGNAL( finished( QNetworkReply * ) ),
             this, SLOT( retrieveData( QNetworkReply * ) ) );
}

void OrsRoutingProvider::retrieveDirections( RouteSkeleton *route )
{
    if ( route->size() < 2 ) {
        return;
    }

    GeoDataCoordinates source = route->source();
    GeoDataCoordinates destination = route->destination();

    QString request = xmlHeader();
    QString unit = "KM";
    QString preference = "Fastest";

    switch ( route->routePreference() ) {
    case RouteSkeleton::CarFastest:
        unit = "KM";
        preference = "Fastest";
        break;
    case RouteSkeleton::CarShortest:
        unit = "KM";
        preference = "Shortest";
        break;
    case RouteSkeleton::Bicycle:
        unit = 'M';
        preference = "Bicycle";
        break;
    case RouteSkeleton::Pedestrian:
        unit = 'M';
        preference = "Pedestrian";
        break;
    }

    request += requestHeader( unit, preference );
    request += requestPoint( StartPoint, source );

    if ( route->size() > 2 ) {
        for ( int i = 1; i < route->size() - 1; ++i ) {
            request += requestPoint( ViaPoint, route->at( i ) );
        }
    }

    request += requestPoint( EndPoint, destination );
    request += requestFooter( route->avoidFeatures() );
    request += xmlFooter();
    //mDebug() << "POST: " << request;

    // Please refrain from making this URI public. To use it outside the scope
    // of marble you need permission from the openrouteservice.org team.
    QUrl url = QUrl( "http://openls.geog.uni-heidelberg.de/osm/eu/routing" );

    QNetworkReply *reply = m_networkAccessManager->post( QNetworkRequest( url ), request.toLatin1() );
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( handleError( QNetworkReply::NetworkError ) ) );
}

void OrsRoutingProvider::retrieveData( QNetworkReply *reply )
{
    QByteArray data = reply->readAll();
    reply->deleteLater();
    //mDebug() << "Download completed: " << data;
    emit routeRetrieved( AbstractRoutingProvider::OpenGIS, data );
}

void OrsRoutingProvider::handleError( QNetworkReply::NetworkError error )
{
    mDebug() << " Error when retrieving openrouteservice.org route: " << error;
}

QString OrsRoutingProvider::xmlHeader() const
{
    QString result = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    result += "<xls:XLS xmlns:xls=\"http://www.opengis.net/xls\" xmlns:sch=\"http://www.ascc.net/xml/schematron\" ";
    result += "xmlns:gml=\"http://www.opengis.net/gml\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" ";
    result += "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ";
    result += "xsi:schemaLocation=\"http://www.opengis.net/xls ";
    result += "http://schemas.opengis.net/ols/1.1.0/RouteService.xsd\" version=\"1.1\" xls:lang=\"%1\">\n";
    result += "<xls:RequestHeader/>\n";
    return result.arg( MarbleLocale::languageCode() );
}

QString OrsRoutingProvider::requestHeader( const QString &unit, const QString &routePreference ) const
{
    QString result = "<xls:Request methodName=\"RouteRequest\" requestID=\"123456789\" version=\"1.1\">\n";
    result += "<xls:DetermineRouteRequest distanceUnit=\"%1\">\n";
    result += "<xls:RoutePlan>\n";
    result += "<xls:RoutePreference>%2</xls:RoutePreference>\n";
    result += "<xls:WayPointList>\n";
    return result.arg( unit ).arg( routePreference );
}

QString OrsRoutingProvider::requestPoint( PointType pointType, const GeoDataCoordinates &coordinates ) const
{
    QString result = "<xls:%1>\n";
    result += "<xls:Position>\n";
    result += "<gml:Point srsName=\"EPSG:4326\">\n";
    result += "<gml:pos>%2 %3</gml:pos>\n";
    result += "</gml:Point>\n";
    result += "</xls:Position>\n";
    result += "</xls:%1>\n";

    result = result.arg( pointType == StartPoint ? "StartPoint" : ( pointType == ViaPoint ? "ViaPoint" : "EndPoint" ) );
    result = result.arg( coordinates.longitude( GeoDataCoordinates::Degree ), 0, 'f', 14 );
    result = result.arg( coordinates.latitude( GeoDataCoordinates::Degree ), 0, 'f', 14 );
    return result;
}

QString OrsRoutingProvider::requestFooter( RouteSkeleton::AvoidFeatures avoidFeatures ) const
{
    QString result = "</xls:WayPointList>\n";

    if ( avoidFeatures != RouteSkeleton::AvoidNone ) {
        result += "<xls:AvoidList>\n"; {
            if ( avoidFeatures & RouteSkeleton::AvoidTollWay )
                result += "<xls:AvoidFeature>Tollway</xls:AvoidFeature>";
        }
        if ( avoidFeatures & RouteSkeleton::AvoidHighway ) {
            result += "<xls:AvoidFeature>Highway</xls:AvoidFeature>";
        }
        result += "</xls:AvoidList>\n";
    }

    result += "</xls:RoutePlan>\n";
    result += "<xls:RouteInstructionsRequest provideGeometry=\"true\" />\n";
    result += "<xls:RouteGeometryRequest/>\n";
    result += "</xls:DetermineRouteRequest>\n";
    result += "</xls:Request>\n";
    return result;
}

QString OrsRoutingProvider::xmlFooter() const
{
    return "</xls:XLS>\n";
}

} // namespace Marble

#include "OrsRoutingProvider.moc"
