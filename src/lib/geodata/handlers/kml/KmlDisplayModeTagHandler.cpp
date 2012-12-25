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
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_displayMode ) );

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_BalloonStyle ) )
    {
        QString mode = parser.readElementText().trimmed();
        GeoDataBalloonStyle::DisplayMode displayMode;
        if ( mode == "default" )
                displayMode = GeoDataBalloonStyle::Default;
        else if ( mode == "hide" )
                displayMode = GeoDataBalloonStyle::Hide;

        parentItem.nodeAs<GeoDataBalloonStyle>()->setDisplayMode( displayMode );

#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_displayMode << "> containing: " << mode
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    }
    return 0;
}

}
}
