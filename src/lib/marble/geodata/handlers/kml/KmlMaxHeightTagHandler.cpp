// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlMaxHeightTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataImagePyramid.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(maxHeight)

GeoNode *KmlmaxHeightTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_maxHeight)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_ImagePyramid)) {
        int maxHeight = parser.readElementText().toInt();

        parentItem.nodeAs<GeoDataImagePyramid>()->setMaxHeight(maxHeight);
    }
    return nullptr;
}

}
}
