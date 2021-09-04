/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlGroupTagHandler.h"

#include "MarbleDebug.h"

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "GeoParser.h"
#include "GeoSceneDocument.h"
#include "GeoSceneSettings.h"
#include "GeoSceneGroup.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Group)

GeoNode* DgmlGroupTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Group)));

    QString name      = parser.attribute(dgmlAttr_name);

    GeoSceneGroup* group = nullptr;

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Settings)) {
        group = new GeoSceneGroup( name );
        parentItem.nodeAs<GeoSceneSettings>()->addGroup( group );
    }

    return group;
}

}
}
