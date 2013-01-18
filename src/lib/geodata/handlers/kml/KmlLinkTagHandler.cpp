//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlLinkTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataLink.h"
#include "GeoDataNetworkLink.h"
#include "GeoDataDocument.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( Link )

GeoNode* KmlLinkTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_Link ) );
    GeoStackItem parentItem = parser.parentElement();
    if ( parentItem.represents( kmlTag_NetworkLink )) {
        return &parentItem.nodeAs<GeoDataNetworkLink>()->link();
    }

    return 0;
}

}
}
