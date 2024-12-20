// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlGridOriginTagHandler.h"

#include "GeoDataImagePyramid.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(gridOrigin)

GeoNode *KmlgridOriginTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_gridOrigin)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_ImagePyramid)) {
        GeoDataImagePyramid::GridOrigin gridOrigin = GeoDataImagePyramid::UpperLeft;
        QString gridOriginText = parser.readElementText();

        if (gridOriginText == QLatin1StringView("lowerLeft")) {
            gridOrigin = GeoDataImagePyramid::LowerLeft;
        } else if (gridOriginText == QLatin1StringView("upperLeft")) {
            gridOrigin = GeoDataImagePyramid::UpperLeft;
        }

        parentItem.nodeAs<GeoDataImagePyramid>()->setGridOrigin(gridOrigin);
    }
    return nullptr;
}

}
}
