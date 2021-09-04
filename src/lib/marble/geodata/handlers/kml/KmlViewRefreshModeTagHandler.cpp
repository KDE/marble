// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlViewRefreshModeTagHandler.h"

#include "GeoDataLink.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER( viewRefreshMode )

GeoNode* KmlviewRefreshModeTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_viewRefreshMode)));

    GeoStackItem parentItem = parser.parentElement();
    if ( parentItem.is<GeoDataLink>() ) {
        QString content = parser.readElementText().trimmed();

        GeoDataLink::ViewRefreshMode mode = GeoDataLink::Never;
        if (content == QLatin1String("onStop")) {
            mode = GeoDataLink::OnStop;
        } else if (content == QLatin1String("onRegion")) {
            mode = GeoDataLink::OnRegion;
        } else if (content == QLatin1String("onRequest")) {
            mode = GeoDataLink::OnRequest;
        }

        parentItem.nodeAs<GeoDataLink>()->setViewRefreshMode( mode );
    }

    return nullptr;
}

}
}
