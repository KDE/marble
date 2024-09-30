/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlPropertyTagHandler.h"

#include "MarbleDebug.h"

#include "DgmlAttributeDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneGroup.h"
#include "GeoSceneProperty.h"
#include "GeoSceneSettings.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Property)

GeoNode *DgmlPropertyTagHandler::parse(GeoParser &parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_Property)));

    QString name = parser.attribute(dgmlAttr_name).trimmed();

    GeoSceneProperty *property = nullptr;

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Settings)) {
        property = new GeoSceneProperty(name);
        parentItem.nodeAs<GeoSceneSettings>()->addProperty(property);
    }
    if (parentItem.represents(dgmlTag_Group)) {
        property = new GeoSceneProperty(name);
        parentItem.nodeAs<GeoSceneGroup>()->addProperty(property);
    }

    return property;
}

}
}
