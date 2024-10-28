/*
    SPDX-FileCopyrightText: 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEOSCENEPARSER_H
#define MARBLE_GEOSCENEPARSER_H

#include "GeoParser.h"
#include <geodata_export.h>

namespace Marble
{

class GeoDocument;
class GeoSceneDocument;

enum GeoSceneSourceType {
    GeoScene_DGML = 0
};

class GEODATA_EXPORT GeoSceneParser : public GeoParser
{
public:
    explicit GeoSceneParser(GeoSceneSourceType source);
    ~GeoSceneParser() override;

private:
    bool isValidElement(const QString &tagName) const override;
    bool isValidRootElement() override;

    GeoDocument *createDocument() const override;
};

// Global helper function for the tag handlers
GeoSceneDocument *geoSceneDoc(GeoParser &parser);

}

#endif
