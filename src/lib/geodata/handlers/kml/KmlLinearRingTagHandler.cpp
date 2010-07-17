/*
    Copyright (C) 2008 Patrick Spendrin <ps_ml@gmx.de>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "KmlLinearRingTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataPolygon.h"
#include "GeoDataMultiGeometry.h"

#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( LinearRing )

GeoNode* KmlLinearRingTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_LinearRing ) );

    GeoStackItem parentItem = parser.parentElement();
    
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_LinearRing << ">"
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif

    if( parentItem.represents( kmlTag_outerBoundaryIs ) ) {
        GeoDataLinearRing linearRing;
        parentItem.nodeAs<GeoDataPolygon>()->setOuterBoundary( linearRing );
        return &parentItem.nodeAs<GeoDataPolygon>()->outerBoundary();

    } else if( parentItem.represents( kmlTag_innerBoundaryIs ) ) {
        GeoDataLinearRing linearRing;
        parentItem.nodeAs<GeoDataPolygon>()->appendInnerBoundary( linearRing );
        return &parentItem.nodeAs<GeoDataPolygon>()->innerBoundaries().last();

    } else if( parentItem.is<GeoDataMultiGeometry>() ) {
        GeoDataLinearRing *linearRing = new GeoDataLinearRing;
        parentItem.nodeAs<GeoDataMultiGeometry>()->append( linearRing );
        return linearRing;
    } else
        return 0;
}

}
}
