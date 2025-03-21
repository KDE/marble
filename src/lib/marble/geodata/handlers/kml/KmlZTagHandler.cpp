// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlZTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataScale.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(z)

GeoNode *KmlzTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_z)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataScale>()) {
        double z = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataScale>()->setZ(z);
    }
    return nullptr;
}

}
}
