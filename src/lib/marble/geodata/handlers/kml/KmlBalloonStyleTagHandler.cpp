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
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( BalloonStyle )

GeoNode* KmlBalloonStyleTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_BalloonStyle)));

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_Style ) ) {
        GeoDataBalloonStyle style;
        KmlObjectTagHandler::parseIdentifiers( parser, &style );

        parentItem.nodeAs<GeoDataStyle>()->setBalloonStyle( style );
        return &parentItem.nodeAs<GeoDataStyle>()->balloonStyle();
    }
    return 0;
}

}
}
