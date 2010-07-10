//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "YoursRoutingProvider.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "RouteSkeleton.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

namespace Marble
{

YoursRoutingProvider::YoursRoutingProvider( QObject *parent ) :
        AbstractRoutingProvider( parent ),
        m_networkAccessManager( new QNetworkAccessManager( this ) )
{
    connect( m_networkAccessManager, SIGNAL( finished( QNetworkReply* ) ),
             this, SLOT( retrieveData( QNetworkReply* ) ) );
}

void YoursRoutingProvider::retrieveDirections( RouteSkeleton *route )
{
    if ( route->size() < 2 ) {
        return;
    }

    GeoDataCoordinates source = route->source();
    GeoDataCoordinates destination = route->destination();

    double fLon = source.longitude( GeoDataCoordinates::Degree );
    double fLat = source.latitude( GeoDataCoordinates::Degree );

    double tLon = destination.longitude( GeoDataCoordinates::Degree );
    double tLat = destination.latitude( GeoDataCoordinates::Degree );

    //QString base = "http://www.yournavigation.org/api/1.0/gosmore.php";
    QString base = "http://nroets.dev.openstreetmap.org/demo/gosmore.php";
    QString args = "?flat=%1&flon=%2&tlat=%3&tlon=%4";
    args = args.arg( fLat, 0, 'f', 6 ).arg( fLon, 0, 'f', 6 ).arg( tLat, 0, 'f', 6 ).arg( tLon, 0, 'f', 6 );
    QString preferences = "&v=motorcar&fast=1&layer=mapnik";
    QString request = base + args + preferences;
    // mDebug() << "GET: " << request;

    QNetworkReply *reply = m_networkAccessManager->get( QNetworkRequest( QUrl( request ) ) );
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( handleError( QNetworkReply::NetworkError ) ) );
}

void YoursRoutingProvider::retrieveData( QNetworkReply *reply )
{
    QByteArray data = reply->readAll();
    reply->deleteLater();
    //mDebug() << "Download completed: " << data;
    emit routeRetrieved( AbstractRoutingProvider::KML, data );
}

void YoursRoutingProvider::handleError( QNetworkReply::NetworkError error )
{
    mDebug() << " Error when retrieving yournavigation.org route: " << error;
}

} // namespace Marble

#include "YoursRoutingProvider.moc"
