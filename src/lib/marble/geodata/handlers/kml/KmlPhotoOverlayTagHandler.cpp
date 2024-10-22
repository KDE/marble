// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlPhotoOverlayTagHandler.h"

#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataParser.h"
#include "GeoDataPhotoOverlay.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(PhotoOverlay)

GeoNode *KmlPhotoOverlayTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_PhotoOverlay)));

    auto overlay = new GeoDataPhotoOverlay;
    KmlObjectTagHandler::parseIdentifiers(parser, overlay);

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Folder) || parentItem.represents(kmlTag_Document) || parentItem.represents(kmlTag_Change)
        || parentItem.represents(kmlTag_Create) || parentItem.represents(kmlTag_Delete)) {
        parentItem.nodeAs<GeoDataContainer>()->append(overlay);
        return overlay;
    } else if (parentItem.qualifiedName().first == QString::fromLatin1(kmlTag_kml)) {
        GeoDataDocument *doc = geoDataDoc(parser);
        doc->append(overlay);
        return overlay;
    } else {
        delete overlay;
        return nullptr;
    }
}

}
}
