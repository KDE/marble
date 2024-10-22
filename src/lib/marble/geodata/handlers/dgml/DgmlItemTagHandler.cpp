/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlItemTagHandler.h"

#include "DgmlAttributeDictionary.h"
#include "DgmlAuxillaryDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneItem.h"
#include "GeoSceneSection.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Item)

GeoNode *DgmlItemTagHandler::parse(GeoParser &parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_Item)));

    QString name = parser.attribute(dgmlAttr_name).trimmed();
    QString checkable = parser.attribute(dgmlAttr_checkable).toLower().trimmed();
    QString connectTo = parser.attribute(dgmlAttr_connect).trimmed();
    int spacing = parser.attribute(dgmlAttr_spacing).toInt();

    GeoSceneItem *item = nullptr;

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Section)) {
        item = new GeoSceneItem(name);
        item->setCheckable(checkable == QString::fromLatin1(dgmlValue_true) || checkable == QString::fromLatin1(dgmlValue_on));
        item->setConnectTo(connectTo);
        item->setSpacing(spacing);
        parentItem.nodeAs<GeoSceneSection>()->addItem(item);
    }

    return item;
}

}
}
