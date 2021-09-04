/*
    SPDX-FileCopyrightText: 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlTextTagHandler.h"

#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneItem.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Text)

GeoNode* DgmlTextTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Text)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Item))
        parentItem.nodeAs<GeoSceneItem>()->setText( parser.readElementText().trimmed() );

    return nullptr;
}

}
}
