//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013       Mayank Madan <maddiemadan@gmail.com>
//

#include "GeoDataFlyTo.h"
#include "GeoParser.h"
#include "KmlFlyToModeTagHandler.h"
#include "KmlElementDictionary.h"
#include "MarbleDebug.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_GX22( flyToMode )

GeoNode* KmlflyToModeTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_flyToMode ) );

    QString content = parser.readElementText().trimmed();

    GeoDataFlyTo::FlyToMode mode;
    if( content == "smooth" ) {
        mode = GeoDataFlyTo::Smooth;
    } else if( content == "bounce" ) {
        mode = GeoDataFlyTo::Bounce;
    } else {
        mDebug() << "Unknown mode " << content << ", using 'bounce' instead.";
        mode = GeoDataFlyTo::Bounce;
    }

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.is<GeoDataFlyTo>()
         && parentItem.represents( kmlTag_flyToMode ) ) {
         parentItem.nodeAs<GeoDataFlyTo>()->setFlyToMode( mode );
    }

    return 0;
}

}
}
