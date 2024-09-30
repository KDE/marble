/*
    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "DgmlProjectionTagHandler.h"

// Marble
#include "DgmlAttributeDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneTileDataset.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Projection)

GeoNode *DgmlProjectionTagHandler::parse(GeoParser &parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_Projection)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (!parentItem.represents(dgmlTag_Texture) && !parentItem.represents(dgmlTag_Vectortile))
        return nullptr;

    // Attribute name, default to "Equirectangular"
    const QString nameStr = parser.attribute(dgmlAttr_name).trimmed();
    if (!nameStr.isEmpty()) {
        GeoSceneAbstractTileProjection::Type tileProjectionType = GeoSceneAbstractTileProjection::Equirectangular;
        if (nameStr == QLatin1StringView("Equirectangular")) {
            tileProjectionType = GeoSceneAbstractTileProjection::Equirectangular;
        } else if (nameStr == QLatin1StringView("Mercator")) {
            tileProjectionType = GeoSceneAbstractTileProjection::Mercator;
        } else {
            parser.raiseWarning(QStringLiteral("Value not allowed for attribute name: %1").arg(nameStr));
        }

        parentItem.nodeAs<GeoSceneTileDataset>()->setTileProjection(tileProjectionType);
    }
    return nullptr;
}

}
}
