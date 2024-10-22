/*
    SPDX-License-Identifier: LGPL-2.1-or-later

    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-FileCopyrightText: 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#include "DgmlVectortileTagHandler.h"

#include <limits>

#include "MarbleDebug.h"

#include "DgmlAttributeDictionary.h"
#include "DgmlAuxillaryDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneLayer.h"
#include "GeoSceneVectorTileDataset.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Vectortile)

GeoNode *DgmlVectortileTagHandler::parse(GeoParser &parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_Vectortile)));

    const QString name = parser.attribute(dgmlAttr_name).trimmed();

    const QString expireStr = parser.attribute(dgmlAttr_expire).trimmed();
    int expire = std::numeric_limits<int>::max();
    if (!expireStr.isEmpty())
        expire = expireStr.toInt();

    GeoSceneTileDataset *texture = nullptr;

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();

    // Check parent type and make sure that the dataSet type
    // matches the backend of the parent layer
    if (parentItem.represents(dgmlTag_Layer) && parentItem.nodeAs<GeoSceneLayer>()->backend() == QString::fromLatin1(dgmlValue_vectortile)) {
        texture = new GeoSceneVectorTileDataset(name);
        texture->setExpire(expire);
        parentItem.nodeAs<GeoSceneLayer>()->addDataset(texture);
    }

    return texture;
}

}
}
