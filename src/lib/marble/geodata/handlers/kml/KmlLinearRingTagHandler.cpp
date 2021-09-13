/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlLinearRingTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataLinearRing.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( LinearRing )

GeoNode* KmlLinearRingTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_LinearRing)));

    GeoStackItem parentItem = parser.parentElement();
    
    if( parentItem.represents( kmlTag_outerBoundaryIs ) ) {
        GeoDataLinearRing linearRing;
        KmlObjectTagHandler::parseIdentifiers( parser, &linearRing );
        parentItem.nodeAs<GeoDataPolygon>()->setOuterBoundary( linearRing );
        return &parentItem.nodeAs<GeoDataPolygon>()->outerBoundary();

    } else if( parentItem.represents( kmlTag_innerBoundaryIs ) ) {
        GeoDataLinearRing linearRing;
        KmlObjectTagHandler::parseIdentifiers( parser, &linearRing );
        parentItem.nodeAs<GeoDataPolygon>()->appendInnerBoundary( linearRing );
        return &parentItem.nodeAs<GeoDataPolygon>()->innerBoundaries().last();

    } else if( parentItem.represents( kmlTag_Placemark ) ) {
        GeoDataLinearRing *linearRing = new GeoDataLinearRing;
        KmlObjectTagHandler::parseIdentifiers( parser, linearRing );
        GeoDataPlacemark *placemark = parentItem.nodeAs<GeoDataPlacemark>();
        placemark->setGeometry( linearRing );
        return placemark->geometry();

    } else if( parentItem.is<GeoDataMultiGeometry>() ) {
        GeoDataLinearRing *linearRing = new GeoDataLinearRing;
        KmlObjectTagHandler::parseIdentifiers( parser, linearRing );
        parentItem.nodeAs<GeoDataMultiGeometry>()->append( linearRing );
        return linearRing;
    } else
        return nullptr;
}

}
}
