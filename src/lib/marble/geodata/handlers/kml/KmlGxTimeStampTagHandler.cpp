// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlGxTimeStampTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataTimeStamp.h"
#include "GeoDataFeature.h"
#include "GeoParser.h"
#include <GeoDataAbstractView.h>
#include "KmlTimeStampTagHandler.h"

namespace Marble
{
namespace kml
{
namespace gx
{
KML_DEFINE_TAG_HANDLER_GX22( TimeStamp )

GeoNode* KmlTimeStampTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_TimeStamp)));
    GeoStackItem parentItem = parser.parentElement();
    GeoDataTimeStamp timestamp;
    KmlObjectTagHandler::parseIdentifiers( parser, &timestamp );
    if ( parentItem.is<GeoDataFeature>() ) {
        parentItem.nodeAs<GeoDataFeature>()->setTimeStamp( timestamp );
        return &parentItem.nodeAs<GeoDataFeature>()->timeStamp();
    } else if ( parentItem.is<GeoDataAbstractView>() ) {
        parentItem.nodeAs<GeoDataAbstractView>()->setTimeStamp( timestamp );
        return &parentItem.nodeAs<GeoDataAbstractView>()->timeStamp();
    }
    return nullptr;
}

}
}
}
