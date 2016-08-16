//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#include "KmlLodTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoParser.h"
#include "GeoDataLod.h"
#include "GeoDataRegion.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( Lod )

GeoNode* KmlLodTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Lod)));

    GeoDataLod lod;
    KmlObjectTagHandler::parseIdentifiers( parser, &lod );
    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_Region ) )
    {
        parentItem.nodeAs<GeoDataRegion>()->setLod( lod );
        return &parentItem.nodeAs<GeoDataRegion>()->lod();
    } else {
        return 0;
    }
}

}
}
