//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlTimeStampTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataTimeStamp.h"
#include "GeoDataFeature.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( TimeStamp )

GeoNode* KmlTimeStampTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_TimeStamp ) );
    GeoStackItem parentItem = parser.parentElement();
    if ( parentItem.is<GeoDataFeature>() ) {
        parentItem.nodeAs<GeoDataFeature>()->setTimeStamp( GeoDataTimeStamp() );
        return &parentItem.nodeAs<GeoDataFeature>()->timeStamp();
    }
    return 0;
}

}
}
