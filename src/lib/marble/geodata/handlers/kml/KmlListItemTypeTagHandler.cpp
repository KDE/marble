// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlListItemTypeTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataListStyle.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( listItemType )

GeoNode* KmllistItemTypeTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_listItemType)));

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_ListStyle ) )
    {
        QString typeText = parser.readElementText().trimmed();
        GeoDataListStyle::ListItemType type = GeoDataListStyle::Check;
        if (typeText == QLatin1String("check")) {
            type = GeoDataListStyle::Check;
        } else if (typeText == QLatin1String("radioFolder")) {
            type = GeoDataListStyle::RadioFolder;
        } else if (typeText == QLatin1String("checkOffOnly")) {
            type = GeoDataListStyle::CheckOffOnly;
        } else if (typeText == QLatin1String("checkHideChildren")) {
            type = GeoDataListStyle::CheckHideChildren;
        }
        else
        {
            mDebug() << "listItemType value is invalid"
                     << "falling back to default - check";
        }
        parentItem.nodeAs<GeoDataListStyle>()->setListItemType( type );
    }
    return nullptr;
}

}
}
