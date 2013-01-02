//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlStateTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataItemIcon.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( state )

GeoNode* KmlstateTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_state ) );

    GeoStackItem parentItem = parser.parentElement();

    GeoDataItemIcon::ItemIconStates itemIconState;

    if ( parentItem.represents( kmlTag_ItemIcon ) )
    {
        QString value = parser.readElementText().trimmed();
        QStringList iconStateTextList = value.split(" ");

        foreach( const QString &value, iconStateTextList ) {
            if ( value == "open" ) {
                itemIconState |= GeoDataItemIcon::Open;
            } else if ( value == "closed" ) {
                itemIconState |= GeoDataItemIcon::Closed;
            } else if ( value == "error" ) {
                itemIconState |= GeoDataItemIcon::Error;
            } else if ( value == "fetching0" ) {
                itemIconState |= GeoDataItemIcon::Fetching0;
            } else if ( value == "fetching1" ) {
                itemIconState |= GeoDataItemIcon::Fetching1;
            } else if ( value == "fetching2" ) {
                itemIconState |= GeoDataItemIcon::Fetching2;
            }
            else {
                mDebug() << "Cannot parse state value" << value;
            }
        }

        parentItem.nodeAs<GeoDataItemIcon>()->setState( itemIconState );

#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_state << "> containing : " << value
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    }
    return 0;
}

}
}
