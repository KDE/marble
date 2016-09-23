//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlDisplayModeTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataBalloonStyle.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( displayMode )

GeoNode* KmldisplayModeTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_displayMode)));

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_BalloonStyle ) )
    {
        QString mode = parser.readElementText().trimmed();
        GeoDataBalloonStyle::DisplayMode displayMode = GeoDataBalloonStyle::Default;
        if (mode == QLatin1String("default")) {
                displayMode = GeoDataBalloonStyle::Default;
        } else if (mode == QLatin1String("hide")) {
                displayMode = GeoDataBalloonStyle::Hide;
        }

        parentItem.nodeAs<GeoDataBalloonStyle>()->setDisplayMode( displayMode );
    }
    return 0;
}

}
}
