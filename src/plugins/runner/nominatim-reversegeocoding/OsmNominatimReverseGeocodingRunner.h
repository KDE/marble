//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//


#ifndef MARBLE_OSMNOMINATIMRUNNER_H
#define MARBLE_OSMNOMINATIMRUNNER_H

#include "ReverseGeocodingRunner.h"
#include "GeoDataCoordinates.h"

#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

class QNetworkReply;
class QDomNode;

namespace Marble
{

class GeoDataExtendedData;

class OsmNominatimRunner : public ReverseGeocodingRunner
{
    Q_OBJECT
public:
    explicit OsmNominatimRunner(QObject *parent = 0);

    ~OsmNominatimRunner();

    // Overriding MarbleAbstractRunner
    virtual void reverseGeocoding( const GeoDataCoordinates &coordinates );

private Q_SLOTS:
    // Forward a result to the search or reverse geocoding handler
    void handleResult( QNetworkReply* );

    void returnNoReverseGeocodingResult();

    void startReverseGeocoding();

private:
    static void extractChildren(const QDomNode &node, GeoDataPlacemark &placemark);

    QNetworkAccessManager m_manager;

    QNetworkRequest m_request;

    GeoDataCoordinates m_coordinates;
};

}

#endif
