//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlAliasTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataAlias.h"
#include "GeoDataResourceMap.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( Alias )

GeoNode* KmlAliasTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Alias)));

    GeoDataAlias alias;
    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_ResourceMap ) ) {
        parentItem.nodeAs<GeoDataResourceMap>()->setAlias(alias);
	    return &parentItem.nodeAs<GeoDataResourceMap>()->alias();
    } else {
        return 0;
    }
}

}
}
