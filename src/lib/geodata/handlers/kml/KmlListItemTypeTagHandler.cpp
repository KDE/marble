//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
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
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_listItemType ) );

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_ListStyle ) )
    {
        QString typeText = parser.readElementText().trimmed();
        GeoDataListStyle::ListItemType type;
        if ( typeText == "check" )
        {
            type = GeoDataListStyle::Check;
        }
        else if ( typeText == "radioFolder" )
        {
            type = GeoDataListStyle::RadioFolder;
        }
        else if ( typeText == "checkOffOnly" )
        {
            type = GeoDataListStyle::CheckOffOnly;
        }
        else if ( typeText == "checkHideChildren" )
        {
            type = GeoDataListStyle::CheckHideChildren;
        }
        else
        {
            mDebug() << "listItemType value is inavlid"
                     << "falling back to default - check";
        }
        parentItem.nodeAs<GeoDataListStyle>()->setListItemType( type );

#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_listItemType << "> containing : " << typeText
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    }
    return 0;
}

}
}
