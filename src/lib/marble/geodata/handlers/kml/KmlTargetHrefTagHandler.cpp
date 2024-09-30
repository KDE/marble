// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlTargetHrefTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataAlias.h"
#include "GeoDataUpdate.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(targetHref)

GeoNode *KmltargetHrefTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_targetHref)));

    GeoStackItem parentItem = parser.parentElement();
    QString content = parser.readElementText().trimmed();

    if (parentItem.is<GeoDataAlias>()) {
        parentItem.nodeAs<GeoDataAlias>()->setTargetHref(content);
    } else if (parentItem.is<GeoDataUpdate>()) {
        parentItem.nodeAs<GeoDataUpdate>()->setTargetHref(content);
    }

    return nullptr;
}

}
}
