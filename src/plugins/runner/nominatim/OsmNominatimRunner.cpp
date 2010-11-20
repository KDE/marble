//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "OsmNominatimRunner.h"

#include "MarbleAbstractRunner.h"
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
    MarbleAbstractRunner( parent ), m_manager( new QNetworkAccessManager (this ) )
{
    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(handleResult(QNetworkReply*)));
}

OsmNominatimRunner::~OsmNominatimRunner()
{
    // nothing to do
}

GeoDataFeature::GeoDataVisualCategory OsmNominatimRunner::category() const
{
    return GeoDataFeature::OsmSite;
}

void OsmNominatimRunner::returnNoResults()
{
    emit searchFinished( QVector<GeoDataPlacemark*>() );
}

void OsmNominatimRunner::returnNoReverseGeocodingResult()
{
    emit reverseGeocodingFinished( m_coordinates, GeoDataPlacemark() );
}

void OsmNominatimRunner::search( const QString &searchTerm )
{    
    QString base = "http://nominatim.openstreetmap.org/search?";
    // @todo: Alternative URI with addressdetails=1 could be used for shorther placemark name
    QString query = "q=%1&format=xml&addressdetails=0&accept-language=%2";
    QString url = QString(base + query).arg(searchTerm).arg(MarbleLocale::languageCode());

    m_searchRequest.setUrl(QUrl(url));
    m_searchRequest.setRawHeader("User-Agent", TinyWebBrowser::userAgent("Browser", "OsmNominatimRunner") );

    // @todo FIXME Must currently be done in the main thread, see bug 257376
    QTimer::singleShot( 0, this, SLOT( startSearch() ) );
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

    m_reverseGeocodingRequest.setUrl(QUrl(url));
    m_reverseGeocodingRequest.setRawHeader("User-Agent", TinyWebBrowser::userAgent("Browser", "OsmNominatimRunner") );

    // @todo FIXME Must currently be done in the main thread, see bug 257376
    QTimer::singleShot( 0, this, SLOT( startReverseGeocoding() ) );
}

void OsmNominatimRunner::startSearch()
{
    QNetworkReply *reply = m_manager->get(m_searchRequest);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(returnNoResults()));
}

void OsmNominatimRunner::startReverseGeocoding()
{
    QNetworkReply *reply = m_manager->get( m_reverseGeocodingRequest );
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(returnNoReverseGeocodingResult()));
}

void OsmNominatimRunner::handleResult( QNetworkReply* reply )
{
    bool const isSearch = reply->url().path().endsWith( "search" );
    if ( isSearch ) {
        handleSearchResult( reply );
    } else {
        handleReverseGeocodingResult( reply );
    }
}


void OsmNominatimRunner::handleSearchResult( QNetworkReply* reply )
{   
    QDomDocument xml;
    if (!xml.setContent(reply->readAll())) {
        qWarning() << "Cannot parse osm nominatim result";
        returnNoResults();
        return;
    }

    QVector<GeoDataPlacemark*> placemarks;
    QDomElement root = xml.documentElement();
    QDomNodeList places = root.elementsByTagName("place");
    for (int i=0; i<places.size(); ++i) {
        QDomNamedNodeMap attributes = places.at(i).attributes();
        QString lon = attributes.namedItem("lon").nodeValue();
        QString lat = attributes.namedItem("lat").nodeValue();
        QString desc = attributes.namedItem("display_name").nodeValue();

        if (!lon.isEmpty() && !lat.isEmpty() && !desc.isEmpty()) {
            GeoDataPlacemark* placemark = new GeoDataPlacemark;
            placemark->setName(desc);
            placemark->setDescription(desc);
            placemark->setCoordinate(lon.toDouble() * DEG2RAD, lat.toDouble() * DEG2RAD);
            placemark->setVisualCategory( category() );
            placemarks << placemark;
        }
    }
    
    emit searchFinished( placemarks );
}

void OsmNominatimRunner::handleReverseGeocodingResult( QNetworkReply* reply )
{
    if ( !reply->bytesAvailable() ) {
        return;
    }

    QDomDocument xml;
    if ( !xml.setContent( reply->readAll() ) ) {
        mDebug() << "Cannot parse osm nominatim result " << xml.toString();
        return;
    }

    QDomElement root = xml.documentElement();
    QDomNodeList places = root.elementsByTagName( "result" );
    if ( places.size() == 1 ) {
        QString address = places.item( 0 ).toElement().text();
        GeoDataPlacemark placemark;
        placemark.setAddress( address );
        placemark.setCoordinate( GeoDataPoint( m_coordinates ) );

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

#include "OsmNominatimRunner.moc"
