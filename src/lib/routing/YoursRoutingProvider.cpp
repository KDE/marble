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

#include "YoursRoutingProvider.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

namespace Marble {

YoursRoutingProvider::YoursRoutingProvider(QObject *parent) :
        AbstractRoutingProvider(parent),
        m_networkAccessManager(new QNetworkAccessManager(this))
{
    connect(m_networkAccessManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(retrieveData(QNetworkReply*)));
}

void YoursRoutingProvider::retrieveDirections(const GeoDataLineString &route)
{
    if (route.size() < 2)
        return;

    GeoDataCoordinates source = route.first();
    GeoDataCoordinates destination = route.last();

    double fLon = source.longitude(GeoDataCoordinates::Degree);
    double fLat = source.latitude(GeoDataCoordinates::Degree);

    double tLon = destination.longitude(GeoDataCoordinates::Degree);
    double tLat = destination.latitude(GeoDataCoordinates::Degree);

    QString base = "http://www.yournavigation.org/api/1.0/gosmore.php";
    QString args = "?flat=%1&flon=%2&tlat=%3&tlon=%4";
    args = args.arg(fLat,0,'f',6).arg(fLon,0,'f',6).arg(tLat,0,'f',6).arg(tLon,0,'f',6);
    QString preferences = "&v=motorcar&fast=1&layer=mapnik&format=kml";
    QString request = base + args + preferences;
    // mDebug() << "GET: " << request;

    QNetworkReply *reply = m_networkAccessManager->get(QNetworkRequest(QUrl(request)));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(handleError(QNetworkReply::NetworkError)));
}

void YoursRoutingProvider::retrieveData(QNetworkReply* reply)
{
    QByteArray data = reply->readAll();
    reply->deleteLater();
    //mDebug() << "Download completed: " << data;
    emit routeRetrieved(AbstractRoutingProvider::KML, data);
}

void YoursRoutingProvider::handleError(QNetworkReply::NetworkError error)
{
    qWarning() << " Error when retrieving yournavigation.org route: " << error;
}

} // namespace Marble

#include "YoursRoutingProvider.moc"
