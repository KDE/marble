/*
    SPDX-FileCopyrightText: 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlThemeTagHandler.h"

#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneHead.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Theme)

GeoNode* DgmlThemeTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Theme)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Head))
        parentItem.nodeAs<GeoSceneHead>()->setTheme( parser.readElementText().trimmed() );

    return nullptr;
}

}
}
