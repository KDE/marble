// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlRefreshIntervalTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataLink.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER( refreshInterval )
    GeoNode *KmlrefreshIntervalTagHandler::parse(GeoParser & parser) const
    {
        Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_refreshInterval)));

        GeoStackItem parentItem = parser.parentElement();
        if ( parentItem.is<GeoDataLink>() ) {
            qreal const refreshInterval = parser.readElementText().trimmed().toDouble();
            parentItem.nodeAs<GeoDataLink>()->setRefreshInterval( refreshInterval );
        }

      return nullptr;
    }
}
}
