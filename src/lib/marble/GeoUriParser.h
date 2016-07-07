//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Levente Kurusa <levex@linux.com>
//

#ifndef MARBLE_GEOURIPARSER_H
#define MARBLE_GEOURIPARSER_H

#include "Planet.h"
#include "GeoDataCoordinates.h"
#include "marble_export.h"

namespace Marble {

/**
 * A class for parsing Geo: URIs.
 * Wikipage: http://en.wikipedia.org/wiki/Geo_URI
 * RFC: 5870 (8 June 2010)
 */
class MARBLE_EXPORT GeoUriParser {

public:
    /**
     * Constructs a new GeoUriParser with the given Geo URI
     */
    explicit GeoUriParser( const QString& geoUri = QString() );

    /**
     * Returns the Geo URI stored in this parser
     */
    QString geoUri() const;
    /**
     * Set the Geo URI to be parsed.
     */
    void setGeoUri( const QString& geoUri );

    /**
     * Returns the coordinates parsed.
     * null is returned when parsing was not successful.
     */
    GeoDataCoordinates coordinates() const;

    /**
     * Returns the Planet on which the coordinates are valid.
     */
    Planet planet() const;

    /**
     * Parse the given Geo URI
     *
     * \returns true iff the GeoURI is valid and was successfully parsed
     */
    bool parse();

private:
    static QString queryValue( const QUrl& url, const QString& key, const QString& secondaryKey=QString() );

    QString m_geoUri;
    GeoDataCoordinates m_coordinates;
    Planet m_planet;
};

}

#endif
