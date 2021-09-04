/*
    SPDX-FileCopyrightText: 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
    SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlHeadingTagHandler.h"

#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneSection.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Heading)

GeoNode* DgmlHeadingTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Heading)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Section))
        parentItem.nodeAs<GeoSceneSection>()->setHeading(parser.readElementText().trimmed());

    return nullptr;
}

}
}
