//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlResourceMapTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataScale.h"
#include "GeoDataGeometry.h"
#include "GeoDataModel.h"
#include "GeoDataResourceMap.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( ResourceMap )

GeoNode* KmlResourceMapTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_ResourceMap)));

    GeoDataResourceMap map;
    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_Model ) ) {
        parentItem.nodeAs<GeoDataModel>()->setResourceMap(map);
	    return &parentItem.nodeAs<GeoDataModel>()->resourceMap();
    } else {
        return 0;
    }
}

}
}
