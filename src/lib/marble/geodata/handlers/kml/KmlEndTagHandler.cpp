// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlEndTagHandler.h"

#include "MarbleDebug.h"

#include "KmlWhenTagHandler.h"
#include "KmlElementDictionary.h"
#include "GeoDataTimeSpan.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( end )

GeoNode* KmlendTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_end)));

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_TimeSpan ) ) {
        QString endString = parser.readElementText().trimmed();
        GeoDataTimeStamp endStamp = KmlwhenTagHandler::parseTimestamp( endString );
        parentItem.nodeAs<GeoDataTimeSpan>()->setEnd( endStamp );
    }

    return nullptr;
}

}
}
