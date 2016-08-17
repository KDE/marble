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
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_state)));

    GeoStackItem parentItem = parser.parentElement();

    GeoDataItemIcon::ItemIconStates itemIconState;

    if ( parentItem.represents( kmlTag_ItemIcon ) )
    {
        QString value = parser.readElementText().trimmed();
        QStringList iconStateTextList = value.split(QLatin1Char(' '));

        foreach( const QString &value, iconStateTextList ) {
            if (value == QLatin1String("open")) {
                itemIconState |= GeoDataItemIcon::Open;
            } else if (value == QLatin1String("closed")) {
                itemIconState |= GeoDataItemIcon::Closed;
            } else if (value == QLatin1String("error")) {
                itemIconState |= GeoDataItemIcon::Error;
            } else if (value == QLatin1String("fetching0")) {
                itemIconState |= GeoDataItemIcon::Fetching0;
            } else if (value == QLatin1String("fetching1")) {
                itemIconState |= GeoDataItemIcon::Fetching1;
            } else if (value == QLatin1String("fetching2")) {
                itemIconState |= GeoDataItemIcon::Fetching2;
            }
            else {
                mDebug() << "Cannot parse state value" << value;
            }
        }

        parentItem.nodeAs<GeoDataItemIcon>()->setState( itemIconState );
    }
    return 0;
}

}
}
