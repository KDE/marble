/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>
    SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlLegendTagHandler.h"

#include "MarbleDebug.h"

#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneDocument.h"
#include "GeoSceneLegend.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Legend)

GeoNode *DgmlLegendTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_Legend)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Document))
        return parentItem.nodeAs<GeoSceneDocument>()->legend();

    return nullptr;
}

}
}
