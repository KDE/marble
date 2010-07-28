//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_OSMNOMINATIMRUNNER_H
#define MARBLE_OSMNOMINATIMRUNNER_H

#include "MarbleAbstractRunner.h"

#include <QtCore/QString>
#include <QtNetwork/QHostInfo>

class QNetworkReply;
class QNetworkAccessManager;
class QDomNodeList;

namespace Marble
{

class OsmNominatimRunner : public MarbleAbstractRunner
{
    Q_OBJECT
public:
    explicit OsmNominatimRunner(QObject *parent = 0);

    ~OsmNominatimRunner();

    // Overriding MarbleAbstractRunner
    GeoDataFeature::GeoDataVisualCategory category() const;

    // Overriding MarbleAbstractRunner
    virtual void reverseGeocoding( const GeoDataCoordinates &coordinates );

    virtual void search( const QString &searchTerm );

private Q_SLOTS:
    // Http search request with nominatim.openstreetmap.org done
    void handleSearchResult( QNetworkReply* );

    // Http reverse geocoding request with nominatim.openstreetmap.org done
    void handleReverseGeocodingResult( QNetworkReply* );

    // No results (or an error)
    void returnNoResults();

private:
    void addData( const QDomNodeList &node, const QString &key, GeoDataExtendedData *extendedData );

    QNetworkAccessManager* m_manager;

    GeoDataCoordinates m_coordinates;
};

}

#endif
