/*
    SPDX-FileCopyrightText: 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEODATAPARSER_H
#define MARBLE_GEODATAPARSER_H

#include "GeoParser.h"
#include "geodata_export.h"

namespace Marble
{

class GeoDocument;
class GeoDataDocument;

enum GeoDataSourceType {
    GeoData_UNKNOWN = -1,
    GeoData_KML = 1,
    GeoData_GeoRSS = 2
};

class GEODATA_EXPORT GeoDataParser : public GeoParser
{
public:
    explicit GeoDataParser(GeoDataSourceType source);
    ~GeoDataParser() override;

private:
    bool isValidElement(const QString &tagName) const override;
    bool isValidRootElement() override;

    GeoDocument *createDocument() const override;
};

// Global helper function for the tag handlers
GEODATA_EXPORT GeoDataDocument *geoDataDoc(GeoParser &parser);

}

#endif
