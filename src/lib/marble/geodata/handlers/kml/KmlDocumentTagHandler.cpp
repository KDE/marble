/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlDocumentTagHandler.h"

#include "GeoDataDocument.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Document)

GeoNode *KmlDocumentTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Document)));

    GeoStackItem parentItem = parser.parentElement();
    if (!(parentItem.qualifiedName().first.isNull() && parentItem.qualifiedName().second.isNull())) {
        // this happens if there is a parent element to the Document tag. We can work around that and simply expect that
        // the new Document tag works like a Folder
        if (parentItem.represents(kmlTag_Folder) || parentItem.represents(kmlTag_Document) || parentItem.represents(kmlTag_Create)) {
            auto document = new GeoDataDocument;
            KmlObjectTagHandler::parseIdentifiers(parser, document);
            parentItem.nodeAs<GeoDataContainer>()->append(document);

            return document;
        } else if (parentItem.qualifiedName().first == QLatin1StringView(kmlTag_kml)) {
            GeoDataDocument *doc = geoDataDoc(parser);
            KmlObjectTagHandler::parseIdentifiers(parser, doc);
            return doc;
        }
    }
    return nullptr;
}

}
}
