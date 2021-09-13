/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlItemTagHandler.h"

#include "MarbleDebug.h"

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "DgmlAuxillaryDictionary.h"
#include "GeoParser.h"
#include "GeoSceneSection.h"
#include "GeoSceneItem.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Item)

GeoNode* DgmlItemTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Item)));

    QString name      = parser.attribute(dgmlAttr_name).trimmed();
    QString checkable = parser.attribute(dgmlAttr_checkable).toLower().trimmed();
    QString connectTo = parser.attribute(dgmlAttr_connect).trimmed();
    int     spacing   = parser.attribute(dgmlAttr_spacing).toInt();

    GeoSceneItem *item = nullptr;

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Section)) {
        item = new GeoSceneItem( name );
        item->setCheckable( checkable == dgmlValue_true || checkable == dgmlValue_on );
        item->setConnectTo( connectTo );
        item->setSpacing( spacing );
        parentItem.nodeAs<GeoSceneSection>()->addItem( item );
    }

    return item;
}

}
}
