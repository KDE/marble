//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlBgColorTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "KmlColorTagHandler.h"
#include "GeoDataBalloonStyle.h"
#include "GeoDataListStyle.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( bgColor )

GeoNode* KmlbgColorTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_bgColor ) );

    GeoStackItem parentItem = parser.parentElement();

    QColor const color = KmlcolorTagHandler::parseColor( parser.readElementText().trimmed() );

    if ( parentItem.represents( kmlTag_BalloonStyle ) )
    {
        parentItem.nodeAs<GeoDataBalloonStyle>()->setBackgroundColor( color );
    }
    else if ( parentItem.represents( kmlTag_ListStyle ) )
    {
        parentItem.nodeAs<GeoDataListStyle>()->setBackgroundColor( color );
    }
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_bgColor << ">"
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS

    return 0;
}

}
}
