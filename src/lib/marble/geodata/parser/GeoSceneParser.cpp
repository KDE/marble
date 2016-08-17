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
#include "GeoSceneParser.h"

// Marble
#include "MarbleDebug.h"

// Geodata
#include "GeoDocument.h"
#include "GeoSceneDocument.h"
#include "GeoTagHandler.h"

// DGML support
#include "DgmlElementDictionary.h"

namespace Marble
{

GeoSceneParser::GeoSceneParser(GeoSceneSourceType source)
    : GeoParser(source)
{
}

GeoSceneParser::~GeoSceneParser()
{
    // nothing to do
}

bool GeoSceneParser::isValidRootElement()
{
    switch ((GeoSceneSourceType) m_source) {
    case GeoScene_DGML:
        return isValidElement(dgml::dgmlTag_Dgml);
    default:
        Q_ASSERT(false);
        return false;
    }
}

bool GeoSceneParser::isValidElement(const QString& tagName) const
{
    if (!GeoParser::isValidElement(tagName))
        return false;

    switch ((GeoSceneSourceType) m_source) {
    case GeoScene_DGML:
        return (namespaceUri() == QLatin1String(dgml::dgmlTag_nameSpace20));
    default:
        break;
    }

    // Should never be reached.
    Q_ASSERT(false);
    return false;
}

GeoDocument* GeoSceneParser::createDocument() const
{
    return new GeoSceneDocument;
}

// Global helper function for the tag handlers
GeoSceneDocument* geoSceneDoc(GeoParser& parser)
{
    GeoDocument* document = parser.activeDocument();
    Q_ASSERT(document->isGeoSceneDocument());
    return static_cast<GeoSceneDocument*>(document);
}

}
