/*
    SPDX-FileCopyrightText: 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "GeoDataParser.h"

#include <QStringView>

// Marble
#include "MarbleDebug.h"

// Geodata
#include "GeoDataDocument.h"
#include "GeoDocument.h"

// TODO: GeoRSS support
// #include "GeoRSSElementDictionary.h"

// KML support
#include "KmlElementDictionary.h"

namespace Marble
{

GeoDataParser::GeoDataParser(GeoDataSourceType source)
    : GeoParser(source)
{
}

GeoDataParser::~GeoDataParser() = default;

bool GeoDataParser::isValidRootElement()
{
    if (m_source == GeoData_UNKNOWN) {
        if (GeoParser::isValidElement(QString::fromLatin1(kml::kmlTag_kml))) {
            m_source = GeoData_KML;
        } else {
            Q_ASSERT(false);
            return false;
        }
    }
    switch ((GeoDataSourceType)m_source) {
    // TODO: case GeoData_GeoRSS:
    case GeoData_KML:
        return isValidElement(QString::fromLatin1(kml::kmlTag_kml));
    default:
        Q_ASSERT(false);
        return false;
    }
}

bool GeoDataParser::isValidElement(const QString &tagName) const
{
    if (!GeoParser::isValidElement(tagName))
        return false;

    switch ((GeoDataSourceType)m_source) {
    // TODO: case GeoData_GeoRSS:
    case GeoData_KML: {
        const QStringView namespaceUri = this->namespaceUri();
        return (namespaceUri == QLatin1StringView(kml::kmlTag_nameSpace20) || namespaceUri == QLatin1StringView(kml::kmlTag_nameSpace21)
                || namespaceUri == QLatin1StringView(kml::kmlTag_nameSpace22) || namespaceUri == QLatin1StringView(kml::kmlTag_nameSpaceOgc22)
                || namespaceUri == QLatin1StringView(kml::kmlTag_nameSpaceGx22) || namespaceUri == QLatin1StringView(kml::kmlTag_nameSpaceMx));
    }
    default:
        break;
    }

    // Should never be reached.
    Q_ASSERT(false);
    return false;
}

GeoDocument *GeoDataParser::createDocument() const
{
    return new GeoDataDocument;
}

// Global helper function for the tag handlers
GeoDataDocument *geoDataDoc(GeoParser &parser)
{
    GeoDocument *document = parser.activeDocument();
    Q_ASSERT(document->isGeoDataDocument());
    return static_cast<GeoDataDocument *>(document);
}

}
