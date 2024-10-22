/*
    SPDX-FileCopyrightText: 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "GeoSceneParser.h"

// Geodata
#include "GeoDocument.h"
#include "GeoSceneDocument.h"

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
    switch ((GeoSceneSourceType)m_source) {
    case GeoScene_DGML:
        return isValidElement(QString::fromLatin1(dgml::dgmlTag_Dgml));
    default:
        Q_ASSERT(false);
        return false;
    }
}

bool GeoSceneParser::isValidElement(const QString &tagName) const
{
    if (!GeoParser::isValidElement(tagName))
        return false;

    switch ((GeoSceneSourceType)m_source) {
    case GeoScene_DGML:
        return (namespaceUri() == QLatin1StringView(dgml::dgmlTag_nameSpace20));
    default:
        break;
    }

    // Should never be reached.
    Q_ASSERT(false);
    return false;
}

GeoDocument *GeoSceneParser::createDocument() const
{
    return new GeoSceneDocument;
}

// Global helper function for the tag handlers
GeoSceneDocument *geoSceneDoc(GeoParser &parser)
{
    GeoDocument *document = parser.activeDocument();
    Q_ASSERT(document->isGeoSceneDocument());
    return static_cast<GeoSceneDocument *>(document);
}

}
