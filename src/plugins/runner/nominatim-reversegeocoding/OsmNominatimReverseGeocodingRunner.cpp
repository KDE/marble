//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "OsmNominatimReverseGeocodingRunner.h"

#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "GeoDataPlacemark.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"
#include "HttpDownloadManager.h"
#include "osm/OsmPlacemarkData.h"

#include <QString>
#include <QUrl>
#include <QTimer>
#include <QNetworkReply>
#include <QDomDocument>

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
    m_request.setRawHeader("User-Agent", HttpDownloadManager::userAgent("Browser", "OsmNominatimRunner") );

    QEventLoop eventLoop;

    QTimer timer;
    timer.setSingleShot( true );
    timer.setInterval( 15000 );

    connect( &timer, SIGNAL(timeout()),
             &eventLoop, SLOT(quit()));
    connect( this, SIGNAL(reverseGeocodingFinished(GeoDataCoordinates,GeoDataPlacemark)),
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
    QDomNodeList places = root.elementsByTagName(QStringLiteral("result"));
    if ( places.size() == 1 ) {
        QString address = places.item( 0 ).toElement().text();
        GeoDataPlacemark placemark;
        placemark.setVisualCategory(GeoDataPlacemark::Coordinate);
        placemark.setAddress( address );
        placemark.setCoordinate( m_coordinates );

        QDomNode details = root.firstChildElement(QStringLiteral("addressparts"));
        extractChildren( details, placemark );

        emit reverseGeocodingFinished( m_coordinates, placemark );
    } else {
        returnNoReverseGeocodingResult();
    }
}

void OsmNominatimRunner::extractChildren(const QDomNode &node, GeoDataPlacemark &placemark)
{
    QMap<QString, QString> tagTranslator;
    tagTranslator["house_number"] = "addr:housenumber";
    tagTranslator["road"] = "addr:street";
    tagTranslator["suburb"] = "addr:suburb";
    tagTranslator["city"] = "addr:city";
    tagTranslator["state_district"] = "addr:district";
    tagTranslator["state"] = "addr:state";
    tagTranslator["postcode"] = "addr:postcode";
    tagTranslator["country_code"] = "addr:country"; // correct mapping
    // @todo Find a proper mapping for those
    //tagTranslator["village"] = "";
    //tagTranslator["town"] = "";

    GeoDataExtendedData data;
    OsmPlacemarkData osmData;
    QDomNodeList nodes = node.childNodes();
    for (int i=0, n=nodes.length(); i<n; ++i) {
        QDomNode child = nodes.item(i);
        data.addValue( GeoDataData( child.nodeName(), child.toElement().text() ) );

        if (tagTranslator.contains(child.nodeName())) {
            QString const key = tagTranslator[child.nodeName()];
            osmData.addTag(key, child.toElement().text());
        }
    }
    placemark.setExtendedData(data);
    placemark.setOsmData(osmData);
}

} // namespace Marble

#include "moc_OsmNominatimReverseGeocodingRunner.cpp"
