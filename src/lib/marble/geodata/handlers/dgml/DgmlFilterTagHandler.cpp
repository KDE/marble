/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>
    SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>
    SPDX-FileCopyrightText: 2008 Henry de Valence <hdevalence@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlFilterTagHandler.h"

#include "MarbleDebug.h"

#include "DgmlAttributeDictionary.h"
#include "DgmlAuxillaryDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneFilter.h"
#include "GeoSceneLayer.h"
#include "GeoSceneMap.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Filter)

GeoNode *DgmlFilterTagHandler::parse(GeoParser &parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_Filter)));

    QString name = parser.attribute(dgmlAttr_name).trimmed();
    QString type = parser.attribute(dgmlAttr_type).toLower().trimmed();

    GeoSceneFilter *filter = nullptr;

    // Checking for parent layer
    GeoStackItem parentItem = parser.parentElement();
    GeoStackItem grandParentItem = parser.parentElement(1);
    if (parentItem.represents(dgmlTag_Layer) && grandParentItem.represents(dgmlTag_Map)) {
        filter = new GeoSceneFilter(name);
        filter->setType(type);
        parentItem.nodeAs<GeoSceneLayer>()->addFilter(filter);
        grandParentItem.nodeAs<GeoSceneMap>()->addFilter(filter);
    }

    return filter;
}

}
}
