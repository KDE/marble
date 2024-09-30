/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>
    SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlLayerTagHandler.h"

#include "MarbleDebug.h"

#include "DgmlAttributeDictionary.h"
#include "DgmlAuxillaryDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneLayer.h"
#include "GeoSceneMap.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Layer)

GeoNode *DgmlLayerTagHandler::parse(GeoParser &parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_Layer)));

    QString name = parser.attribute(dgmlAttr_name).trimmed();
    QString backend = parser.attribute(dgmlAttr_backend).toLower().trimmed();
    QString role = parser.attribute(dgmlAttr_role).toLower().trimmed();

    GeoSceneLayer *layer = nullptr;

    // Checking for parent layer
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Map)) {
        layer = new GeoSceneLayer(name);
        layer->setBackend(backend);
        layer->setRole(role);
        parentItem.nodeAs<GeoSceneMap>()->addLayer(layer);
    }

    return layer;
}

}
}
