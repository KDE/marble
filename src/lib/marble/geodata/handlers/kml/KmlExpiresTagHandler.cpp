// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlExpiresTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataNetworkLinkControl.h"
#include "GeoDataParser.h"

#include <QDateTime>

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( expires )

GeoNode* KmlexpiresTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_expires)));

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_NetworkLinkControl ) )
    {
        QDateTime time = QDateTime::fromString( parser.readElementText(), Qt::ISODate );

        parentItem.nodeAs<GeoDataNetworkLinkControl>()->setExpires( time );
    }

    return nullptr;
}

}
}
