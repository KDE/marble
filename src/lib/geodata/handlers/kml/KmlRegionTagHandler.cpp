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
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_Region ) );

    GeoDataRegion region;

    GeoStackItem parentItem = parser.parentElement();
#ifdef DEBUG_TAGS
    mDebug() << "Parsed <" << kmlTag_Region << ">"
             << " parent item name: " << parentItem.qualifiedName().first;
#endif

    if( parentItem.is<GeoDataFeature>() ) {
        parentItem.nodeAs<GeoDataFeature>()->setRegion( region );
        return &parentItem.nodeAs<GeoDataFeature>()->region();
    } else {
        return 0;
    }
}

}
}
