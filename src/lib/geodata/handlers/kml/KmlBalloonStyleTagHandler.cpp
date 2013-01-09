//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlBalloonStyleTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataBalloonStyle.h"
#include "GeoDataStyle.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( BalloonStyle )

GeoNode* KmlBalloonStyleTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_BalloonStyle ) );

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_Style ) ) {
        GeoDataBalloonStyle style;

        parentItem.nodeAs<GeoDataStyle>()->setBalloonStyle( style );
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_BalloonStyle << "> containing: " << &parentItem.nodeAs<GeoDataStyle>()->balloonStyle()
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
        return &parentItem.nodeAs<GeoDataStyle>()->balloonStyle();
    }
    return 0;
}

}
}
