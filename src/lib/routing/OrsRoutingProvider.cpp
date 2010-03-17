/*
    Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "OrsRoutingProvider.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "RouteSkeleton.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

namespace Marble {

OrsRoutingProvider::OrsRoutingProvider(QObject *parent) :
        AbstractRoutingProvider(parent),
        m_networkAccessManager(new QNetworkAccessManager(this))
{
    connect(m_networkAccessManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(retrieveData(QNetworkReply*)));
}

void OrsRoutingProvider::retrieveDirections(RouteSkeleton* route)
{
    if (route->size() < 2)
        return;

    GeoDataCoordinates source = route->source();
    GeoDataCoordinates destination = route->destination();

    QString request = xmlHeader();
    request += requestHeader(Meter, Fastest);
    request += requestPoint(StartPoint, source);

    if (route->size() > 2) {
        for (int i=1; i<route->size()-1; ++i) {
            request += requestPoint(ViaPoint, route->at(i));
        }
    }

    request += requestPoint(EndPoint, destination);
    request += requestFooter();
    request += xmlFooter();
    //mDebug() << "POST: " << request;

    // Please refrain from making this URI public. To use it outside the scope
    // of marble you need permission from the openrouteservice.org team.
    QUrl url = QUrl("http://openls.geog.uni-heidelberg.de/osm/eu/routing");

    QNetworkReply *reply = m_networkAccessManager->post(QNetworkRequest(url), request.toLatin1());
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(handleError(QNetworkReply::NetworkError)));
}

void OrsRoutingProvider::retrieveData(QNetworkReply* reply)
{
    QByteArray data = reply->readAll();
    reply->deleteLater();
    //mDebug() << "Download completed: " << data;
    emit routeRetrieved(AbstractRoutingProvider::OpenGIS, data);
}

void OrsRoutingProvider::handleError(QNetworkReply::NetworkError error)
{
    qWarning() << " Error when retrieving openrouteservice.org route: " << error;
}

QString OrsRoutingProvider::xmlHeader() const
{
    QString result = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    result += "<xls:XLS xmlns:xls=\"http://www.opengis.net/xls\" xmlns:sch=\"http://www.ascc.net/xml/schematron\" ";
    result += "xmlns:gml=\"http://www.opengis.net/gml\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" ";
    result += "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ";
    result += "xsi:schemaLocation=\"http://www.opengis.net/xls ";
    result += "http://schemas.opengis.net/ols/1.1.0/RouteService.xsd\" version=\"1.1\" xls:lang=\"en\">\n";
    result += "<xls:RequestHeader/>\n";
    return result;
}

QString OrsRoutingProvider::requestHeader(DistanceUnit unit, Preference preference) const
{
    QString result = "<xls:Request methodName=\"RouteRequest\" requestID=\"123456789\" version=\"1.1\">\n";
    result += "<xls:DetermineRouteRequest distanceUnit=\"%1\">\n";
    result += "<xls:RoutePlan>\n";
    result += "<xls:RoutePreference>%2</xls:RoutePreference>\n";
    result += "<xls:WayPointList>\n";
    QString pref = preference == Fastest ? "Fastest" : preference == Shortest ? "Shortest" : "Pedestrian";
    return result.arg(unit == MilesFeet ? "M" : "KM").arg( pref );
}

QString OrsRoutingProvider::requestPoint(PointType pointType, const GeoDataCoordinates &coordinates) const
{
    QString result = "<xls:%1>\n";
    result += "<xls:Position>\n";
    result += "<gml:Point srsName=\"EPSG:4326\">\n";
    result += "<gml:pos>%2 %3</gml:pos>\n";
    result += "</gml:Point>\n";
    result += "</xls:Position>\n";
    result += "</xls:%1>\n";

    result = result.arg(pointType == StartPoint ? "StartPoint" : ( pointType == ViaPoint ? "ViaPoint" : "EndPoint"));
    result = result.arg( coordinates.longitude(GeoDataCoordinates::Degree), 0, 'f', 14 );
    result = result.arg( coordinates.latitude(GeoDataCoordinates::Degree), 0, 'f', 14 );
    return result;
}

QString OrsRoutingProvider::requestFooter() const
{
    QString result = "</xls:WayPointList>\n";
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
