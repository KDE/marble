/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlZoomTagHandler.h"

#include "MarbleDebug.h"

#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneHead.h"
#include "GeoSceneZoom.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Zoom)

GeoNode *DgmlZoomTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_Zoom)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Head))
        return parentItem.nodeAs<GeoSceneHead>()->zoom();

    return nullptr;
}

}
}
