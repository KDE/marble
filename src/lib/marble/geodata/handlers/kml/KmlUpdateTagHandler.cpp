// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlUpdateTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataNetworkLinkControl.h"
#include "GeoDataUpdate.h"
#include "GeoDataAnimatedUpdate.h"
#include "GeoDataParser.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( Update )

GeoNode* KmlUpdateTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Update)));

    GeoDataUpdate *update = new GeoDataUpdate;
    KmlObjectTagHandler::parseIdentifiers( parser, update );
    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_NetworkLinkControl ) ) {
        return &parentItem.nodeAs<GeoDataNetworkLinkControl>()->update();
    } else if ( parentItem.represents( kmlTag_AnimatedUpdate ) ) {
        parentItem.nodeAs<GeoDataAnimatedUpdate>()->setUpdate( update );
        return update;
    }
    return nullptr;
}

}
}
