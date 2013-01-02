//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlListStyleTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataListStyle.h"
#include "GeoDataStyle.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( ListStyle )

GeoNode* KmlListStyleTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_ListStyle ) );

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_Style ) ) {
        GeoDataListStyle style;

        parentItem.nodeAs<GeoDataStyle>()->setListStyle( style );
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_ListStyle << "> containing: " << &parentItem.nodeAs<GeoDataStyle>()->listStyle()
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
        return &parentItem.nodeAs<GeoDataStyle>()->listStyle();
    }
    return 0;
}

}
}
