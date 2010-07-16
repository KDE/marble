//
// This file is part of the Marble Desktop Globe.
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
#include "TinyWebBrowser.h"

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtXml/QDomDocument>

namespace Marble
{

OsmNominatimRunner::OsmNominatimRunner( QObject *parent ) :
        MarbleAbstractRunner( parent ), m_manager(0)
{
    // nothing to do
}

OsmNominatimRunner::~OsmNominatimRunner()
{
    quit();
    m_manager->deleteLater();

    if (!wait(5000)) {
        qWarning() << "Unable to terminate OsmNominatimRunner thread properly.";
    }
}

GeoDataFeature::GeoDataVisualCategory OsmNominatimRunner::category() const
{
    return GeoDataFeature::OsmSite;
}

void OsmNominatimRunner::returnNoResults()
{
    emit runnerFinished( this, QVector<GeoDataPlacemark*>() );
}

void OsmNominatimRunner::run()
{    
    QString base = "http://nominatim.openstreetmap.org/search?q=%1&format=xml";
    // @todo: Alternative URI with addressdetails=1 could be used for shorther placemark name
    QString query = "q=%1&format=xml&addressdetails=0&accept-language=%2";
    QString url = QString(base + query).arg(m_input).arg(MarbleLocale::languageCode());

    m_manager = new QNetworkAccessManager;
    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(handleHttpReply(QNetworkReply*)));

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", TinyWebBrowser::userAgent("Browser", "OsmNominatimRunner") );

    QNetworkReply *reply = m_manager->get(QNetworkRequest(request));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(returnNoResults()));

    exec();
}

void OsmNominatimRunner::handleHttpReply( QNetworkReply* reply )
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
            GeoDataPlacemark *placemark = new GeoDataPlacemark;
            placemark->setName(desc);
            placemark->setDescription(desc);
            placemark->setCoordinate(lon.toDouble() * DEG2RAD, lat.toDouble() * DEG2RAD);
            placemark->setVisualCategory( category() );
            placemarks << placemark;
        }
    }
    
    emit runnerFinished( this, placemarks );
}

} // namespace Marble

#include "OsmNominatimRunner.moc"
