/*
    Copyright (C) 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/


// Own
#include "GeoDataParser.h"

// Marble
#include "MarbleDebug.h"

// Geodata
#include "GeoDataDocument.h"
#include "GeoDocument.h"
#include "GeoTagHandler.h"


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

GeoDataParser::~GeoDataParser()
{
}

bool GeoDataParser::isValidRootElement()
{
    if (m_source == GeoData_UNKNOWN)
    {
        if (GeoParser::isValidElement(kml::kmlTag_kml))
        {
            m_source = GeoData_KML;
        }
        else
        {
            Q_ASSERT(false);
            return false;
        }
    }
    switch ((GeoDataSourceType) m_source) {
    // TODO: case GeoData_GeoRSS:
    case GeoData_KML:
        return isValidElement(kml::kmlTag_kml);
    default:
        Q_ASSERT(false);
        return false;
    }
}

bool GeoDataParser::isValidElement(const QString& tagName) const
{
    if (!GeoParser::isValidElement(tagName))
        return false;

    switch ((GeoDataSourceType) m_source) {
    // TODO: case GeoData_GeoRSS:
    case GeoData_KML:
        return (namespaceUri() == kml::kmlTag_nameSpace20 || 
                namespaceUri() == kml::kmlTag_nameSpace21 || 
                namespaceUri() == kml::kmlTag_nameSpace22 ||
                namespaceUri() == kml::kmlTag_nameSpaceOgc22);
    default:
        break;
    }

    // Should never be reached.
    Q_ASSERT(false);
    return false;
}

GeoDocument* GeoDataParser::createDocument() const
{
    return new GeoDataDocument;
}

// Global helper function for the tag handlers
GeoDataDocument* geoDataDoc(GeoParser& parser)
{
    GeoDocument* document = parser.activeDocument();
    Q_ASSERT(document->isGeoDataDocument());
    return static_cast<GeoDataDocument*>(document);
}

}
