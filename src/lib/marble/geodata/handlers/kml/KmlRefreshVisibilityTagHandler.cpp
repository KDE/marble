// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlRefreshVisibilityTagHandler.h"
#include "GeoDataNetworkLink.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "MarbleDebug.h"
#include "MarbleGlobal.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER(refreshVisibility)

GeoNode *KmlrefreshVisibilityTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_refreshVisibility)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.is<GeoDataNetworkLink>()) {
        QString content = parser.readElementText().trimmed();
        auto networkLink = parentItem.nodeAs<GeoDataNetworkLink>();
        networkLink->setRefreshVisibility(content == QLatin1StringView("1"));
    }

    return nullptr;
}

}
}
