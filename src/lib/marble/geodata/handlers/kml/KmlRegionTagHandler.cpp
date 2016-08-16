//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#include "KmlRegionTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataFeature.h"
#include "GeoParser.h"
#include "GeoDataRegion.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( Region )

GeoNode* KmlRegionTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Region)));

    GeoDataRegion region;
    KmlObjectTagHandler::parseIdentifiers( parser, &region );

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.is<GeoDataFeature>() ) {
        parentItem.nodeAs<GeoDataFeature>()->setRegion( region );
        return &parentItem.nodeAs<GeoDataFeature>()->region();
    } else {
        return 0;
    }
}

}
}
