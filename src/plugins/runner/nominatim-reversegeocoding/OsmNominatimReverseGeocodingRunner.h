//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//


#ifndef MARBLE_OSMNOMINATIMREVERSEGEOCODINGRUNNER_H
#define MARBLE_OSMNOMINATIMREVERSEGEOCODINGRUNNER_H

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
    explicit OsmNominatimRunner(QObject *parent = nullptr);

    ~OsmNominatimRunner() override;

    // Overriding MarbleAbstractRunner
    void reverseGeocoding( const GeoDataCoordinates &coordinates ) override;

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
