//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "OsmNominatimSearchRunner.h"

#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataExtendedData.h"
#include "HttpDownloadManager.h"

#include <QString>
#include <QVector>
#include <QUrl>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDomDocument>

namespace Marble
{

OsmNominatimRunner::OsmNominatimRunner( QObject *parent ) :
    SearchRunner( parent ),
    m_manager()
{
    connect(&m_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(handleResult(QNetworkReply*)));
}

OsmNominatimRunner::~OsmNominatimRunner()
{
    // nothing to do
}

void OsmNominatimRunner::returnNoResults()
{
    emit searchFinished( QVector<GeoDataPlacemark*>() );
}

void OsmNominatimRunner::search( const QString &searchTerm, const GeoDataLatLonBox &preferred )
{    
    QString base = "http://nominatim.openstreetmap.org/search?";
    QString query = "q=%1&format=xml&addressdetails=1&accept-language=%2";
    QString url = QString(base + query).arg(searchTerm).arg(MarbleLocale::languageCode());
    if( !preferred.isEmpty() ) {
        GeoDataCoordinates::Unit deg = GeoDataCoordinates::Degree;
        QString viewbox( "&viewbox=%1,%2,%3,%4&bounded=1" ); // left, top, right, bottom
        url += viewbox.arg(preferred.west(deg))
                      .arg(preferred.north(deg))
                      .arg(preferred.east(deg))
                      .arg(preferred.south(deg));

    }
    m_request.setUrl(QUrl(url));
    m_request.setRawHeader("User-Agent", HttpDownloadManager::userAgent("Browser", "OsmNominatimRunner") );

    QEventLoop eventLoop;

    QTimer timer;
    timer.setSingleShot( true );
    timer.setInterval( 15000 );

    connect( &timer, SIGNAL(timeout()),
             &eventLoop, SLOT(quit()));
    connect( this, SIGNAL(searchFinished(QVector<GeoDataPlacemark*>)),
             &eventLoop, SLOT(quit()) );

    // @todo FIXME Must currently be done in the main thread, see bug 257376
    QTimer::singleShot( 0, this, SLOT(startSearch()) );
    timer.start();

    eventLoop.exec();
}

void OsmNominatimRunner::startSearch()
{
    QNetworkReply *reply = m_manager.get( m_request );
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(returnNoResults()));
}


void OsmNominatimRunner::handleResult( QNetworkReply* reply )
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
        QDomNode place = places.at(i);
        QDomNamedNodeMap attributes = place.attributes();
        QString lon = attributes.namedItem("lon").nodeValue();
        QString lat = attributes.namedItem("lat").nodeValue();
        QString desc = attributes.namedItem("display_name").nodeValue();
        QString key = attributes.namedItem("class").nodeValue();
        QString value = attributes.namedItem("type").nodeValue();

        QString name = place.firstChildElement(value).text();
        QString road = place.firstChildElement("road").text();

        QString city = place.firstChildElement("city").text();
        if( city.isEmpty() ) {
            city = place.firstChildElement("town").text();
            if( city.isEmpty() ) {
                city = place.firstChildElement("village").text();
            } if( city.isEmpty() ) {
                city = place.firstChildElement("hamlet").text();
            }
        }

        QString administrative = place.firstChildElement("county").text();
        if( administrative.isEmpty() ) {
            administrative = place.firstChildElement("region").text();
            if( administrative.isEmpty() ) {
                administrative = place.firstChildElement("state").text();
            }
        }

        QString country = place.firstChildElement("country").text();

        QString description;
        for (int i=0; i<place.childNodes().size(); ++i) {
            QDomElement item = place.childNodes().at(i).toElement();
            description += item.nodeName() + ':' + item.text() + '\n';
        }
        description += "Category: " + key + '/' + value;

        if (!lon.isEmpty() && !lat.isEmpty() && !desc.isEmpty()) {
            QString placemarkName;
            GeoDataPlacemark* placemark = new GeoDataPlacemark;
            // try to provide 2 fields
            if (!name.isEmpty()) {
                placemarkName = name;
            }
            if (!road.isEmpty() && road != placemarkName ) {
                if( !placemarkName.isEmpty() ) {
                    placemarkName += ", ";
                }
                placemarkName += road;
            }
            if (!city.isEmpty() && !placemarkName.contains(",") && city != placemarkName) {
                if( !placemarkName.isEmpty() ) {
                    placemarkName += ", ";
                }
                placemarkName += city;
            }
            if (!administrative.isEmpty()&& !placemarkName.contains(",") && administrative != placemarkName) {
                if( !placemarkName.isEmpty() ) {
                    placemarkName += ", ";
                }
                placemarkName += administrative;
            }
            if (!country.isEmpty()&& !placemarkName.contains(",") && country != placemarkName) {
                if( !placemarkName.isEmpty() ) {
                    placemarkName += ", ";
                }
                placemarkName += country;
            }
            if (placemarkName.isEmpty()) {
                placemarkName = desc;
            }
            placemark->setName( placemarkName );
            placemark->setDescription(description);
            placemark->setCoordinate( lon.toDouble(), lat.toDouble(), 0, GeoDataCoordinates::Degree );
            GeoDataFeature::GeoDataVisualCategory category = GeoDataFeature::OsmVisualCategory( key + '=' + value );
            placemark->setVisualCategory( category );
            placemarks << placemark;
        }
    }
    
    emit searchFinished( placemarks );
}

} // namespace Marble

#include "moc_OsmNominatimSearchRunner.cpp"
