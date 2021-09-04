// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlFlyToViewTagHandler.h"

#include "GeoDataNetworkLink.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( flyToView )

GeoNode* KmlflyToViewTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_flyToView)));

    GeoStackItem parentItem = parser.parentElement();
    if( parentItem.is<GeoDataNetworkLink>() ) {
        QString content = parser.readElementText().trimmed();
        GeoDataNetworkLink* networkLink = parentItem.nodeAs<GeoDataNetworkLink>();
        networkLink->setFlyToView(content == QLatin1String("1"));
    }

    return nullptr;
}

}
}
