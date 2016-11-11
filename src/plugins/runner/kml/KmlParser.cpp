//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "KmlParser.h"
#include "KmlElementDictionary.h"

namespace Marble {

KmlParser::KmlParser()
    : GeoParser( 0 )
{
}

KmlParser::~KmlParser()
{
}

bool KmlParser::isValidRootElement()
{
    return isValidElement(kml::kmlTag_kml);
}

bool KmlParser::isValidElement(const QString& tagName) const
{
    if (!GeoParser::isValidElement(tagName))
        return false;

    return (namespaceUri() == kml::kmlTag_nameSpace20   ||
            namespaceUri() == kml::kmlTag_nameSpace21   ||
            namespaceUri() == kml::kmlTag_nameSpace22   ||
            namespaceUri() == kml::kmlTag_nameSpaceGx22 ||
            namespaceUri() == kml::kmlTag_nameSpaceOgc22||
            namespaceUri() == kml::kmlTag_nameSpaceMx );
}

GeoDocument* KmlParser::createDocument() const
{
    return new KmlDocument;
}

}
