// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlBeginTagHandler.h"

#include "MarbleDebug.h"

#include "KmlWhenTagHandler.h"
#include "KmlElementDictionary.h"
#include "GeoDataTimeSpan.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( begin )

GeoNode* KmlbeginTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_begin)));

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_TimeSpan ) ) {
        QString beginString = parser.readElementText().trimmed();
        GeoDataTimeStamp beginStamp = KmlwhenTagHandler::parseTimestamp( beginString );
        parentItem.nodeAs<GeoDataTimeSpan>()->setBegin( beginStamp );
    }

    return nullptr;
}

}
}
