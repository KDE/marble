//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlGxTimeSpanTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataTimeSpan.h"
#include "GeoDataFeature.h"
#include "GeoDataAbstractView.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
namespace gx
{

KML_DEFINE_TAG_HANDLER_GX22( TimeSpan )

GeoNode* KmlTimeSpanTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_TimeSpan)));

    GeoStackItem parentItem = parser.parentElement();
    if ( parentItem.is<GeoDataFeature>() ) {
        GeoDataTimeSpan timeSpan;
        KmlObjectTagHandler::parseIdentifiers( parser, &timeSpan );
        parentItem.nodeAs<GeoDataFeature>()->setTimeSpan( timeSpan );
        return &parentItem.nodeAs<GeoDataFeature>()->timeSpan();
    } else if ( parentItem.is<GeoDataAbstractView>() ) {
        GeoDataTimeSpan timeSpan;
        KmlObjectTagHandler::parseIdentifiers( parser, &timeSpan );
        parentItem.nodeAs<GeoDataAbstractView>()->setTimeSpan( timeSpan );
        return &parentItem.nodeAs<GeoDataAbstractView>()->timeSpan();
    }

    return 0;
}

}
}
}
