//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DATABASEQUERY_H
#define MARBLE_DATABASEQUERY_H

#include "GeoDataCoordinates.h"
#include "OsmPlacemark.h"

#include <QtCore/QList>
#include <QtCore/QString>

namespace Marble {

class MarbleModel;
class GeoDataLatLonAltBox;

/**
  * Parse result of a user's search term
  */
class DatabaseQuery
{
public:
    enum QueryType {
        AddressSearch,
        CategorySearch,
        BroadSearch
    };

    enum ResultFormat {
        AddressFormat,
        DistanceFormat
    };

    DatabaseQuery( MarbleModel* model, const QString &searchTerm, const GeoDataLatLonAltBox preferred );

    QueryType queryType() const;

    ResultFormat resultFormat() const;

    QString street() const;

    QString houseNumber() const;

    QString region() const;

    QString searchTerm() const;

    OsmPlacemark::OsmCategory category() const;

    GeoDataCoordinates position() const;

private:
    bool isPointOfInterest( const QString &category );

    QueryType m_queryType;
    ResultFormat m_resultFormat;

    QString m_street;
    QString m_houseNumber;
    QString m_region;

    QString m_searchTerm;

    GeoDataCoordinates m_position;

    OsmPlacemark::OsmCategory m_category;
};

}

#endif // MARBLE_DATABASEQUERY_H
