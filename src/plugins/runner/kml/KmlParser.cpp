// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#include "GeoDataDocument.h"
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

    return (namespaceUri() == QStringView(QString::fromUtf8(kml::kmlTag_nameSpace20))   ||
            namespaceUri() == QStringView(QString::fromUtf8(kml::kmlTag_nameSpace21))   ||
            namespaceUri() == QStringView(QString::fromUtf8(kml::kmlTag_nameSpace22))   ||
            namespaceUri() == QStringView(QString::fromUtf8(kml::kmlTag_nameSpaceGx22)) ||
            namespaceUri() == QStringView(QString::fromUtf8(kml::kmlTag_nameSpaceOgc22))||
            namespaceUri() == QStringView(QString::fromUtf8(kml::kmlTag_nameSpaceMx)) );
}

GeoDocument* KmlParser::createDocument() const
{
    return new GeoDataDocument;
}

}
