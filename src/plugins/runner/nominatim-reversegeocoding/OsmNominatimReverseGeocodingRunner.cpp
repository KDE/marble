//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "OsmNominatimReverseGeocodingRunner.h"

#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataExtendedData.h"
#include "TinyWebBrowser.h"

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QUrl>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtXml/QDomDocument>

namespace Marble
{

OsmNominatimRunner::OsmNominatimRunner( QObject *parent ) :
    ReverseGeocodingRunner( parent ),
    m_manager(this)
{
    connect(&m_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(handleResult(QNetworkReply*)));
}

OsmNominatimRunner::~OsmNominatimRunner()
{
    // nothing to do
}


void OsmNominatimRunner::returnNoReverseGeocodingResult()
{
    emit reverseGeocodingFinished( m_coordinates, GeoDataPlacemark() );
}

void OsmNominatimRunner::reverseGeocoding( const GeoDataCoordinates &coordinates )
{
    m_coordinates = coordinates;
    QString base = "http://nominatim.openstreetmap.org/reverse?format=xml&addressdetails=1";
    // @todo: Alternative URI with addressdetails=1 could be used for shorther placemark name
    QString query = "&lon=%1&lat=%2&accept-language=%3";
    double lon = coordinates.longitude( GeoDataCoordinates::Degree );
    double lat = coordinates.latitude( GeoDataCoordinates::Degree );
    QString url = QString( base + query ).arg( lon ).arg( lat ).arg( MarbleLocale::languageCode() );

    m_request.setUrl(QUrl(url));
    m_request.setRawHeader("User-Agent", TinyWebBrowser::userAgent("Browser", "OsmNominatimRunner") );

    QEventLoop eventLoop;

    QTimer timer;
    timer.setSingleShot( true );
    timer.setInterval( 15000 );

    connect( &timer, SIGNAL(timeout()),
             &eventLoop, SLOT(quit()));
    connect( this, SIGNAL(reverseGeocodingFinished(GeoDataCoordinates, GeoDataPlacemark)),
             &eventLoop, SLOT(quit()) );

    // @todo FIXME Must currently be done in the main thread, see bug 257376
    QTimer::singleShot( 0, this, SLOT(startReverseGeocoding()) );
    timer.start();

    eventLoop.exec();
}

void OsmNominatimRunner::startReverseGeocoding()
{
    QNetworkReply *reply = m_manager.get( m_request );
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(returnNoReverseGeocodingResult()));
}

void OsmNominatimRunner::handleResult( QNetworkReply* reply )
{
    if ( !reply->bytesAvailable() ) {
        returnNoReverseGeocodingResult();
        return;
    }

    QDomDocument xml;
    if ( !xml.setContent( reply->readAll() ) ) {
        mDebug() << "Cannot parse osm nominatim result " << xml.toString();
        returnNoReverseGeocodingResult();
        return;
    }

    QDomElement root = xml.documentElement();
    QDomNodeList places = root.elementsByTagName( "result" );
    if ( places.size() == 1 ) {
        QString address = places.item( 0 ).toElement().text();
        GeoDataPlacemark placemark;
        placemark.setAddress( address );
        placemark.setCoordinate( m_coordinates );

        QDomNodeList details = root.elementsByTagName( "addressparts" );
        if ( details.size() == 1 ) {
            GeoDataExtendedData extendedData;
            addData( details, "road", &extendedData );
            addData( details, "house_number", &extendedData );
            addData( details, "village", &extendedData );
            addData( details, "city", &extendedData );
            addData( details, "county", &extendedData );
            addData( details, "state", &extendedData );
            addData( details, "postcode", &extendedData );
            addData( details, "country", &extendedData );
            placemark.setExtendedData( extendedData );
        }

        emit reverseGeocodingFinished( m_coordinates, placemark );
    } else {
        returnNoReverseGeocodingResult();
    }
}

void OsmNominatimRunner::addData( const QDomNodeList &node, const QString &key, GeoDataExtendedData *extendedData )
{
    QDomNodeList child = node.item( 0 ).toElement().elementsByTagName( key );
    if ( child.size() > 0 ) {
        QString text = child.item( 0 ).toElement().text();
        extendedData->addValue( GeoDataData( key, text ) );
    }
}

} // namespace Marble

#include "OsmNominatimReverseGeocodingRunner.moc"
