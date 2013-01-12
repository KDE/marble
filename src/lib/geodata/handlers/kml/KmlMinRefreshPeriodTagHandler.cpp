//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlMinRefreshPeriodTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataNetworkLinkControl.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( minRefreshPeriod )

GeoNode* KmlminRefreshPeriodTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_minRefreshPeriod ) );

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_NetworkLinkControl ) )
    {
        qreal minRefreshPeriod = parser.readElementText().toDouble();

        parentItem.nodeAs<GeoDataNetworkLinkControl>()->setMinRefreshPeriod( minRefreshPeriod );
    }

    return 0;
}

}
}
