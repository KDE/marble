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

#include "KmlHotSpotTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataIconStyle.h"
#include "GeoDataHotSpot.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( hotSpot )

GeoNode* KmlhotSpotTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_hotSpot ) );

    GeoStackItem parentItem = parser.parentElement();
    
    if ( parentItem.represents( kmlTag_IconStyle ) ) {
        QPointF pf( parser.attribute( "x" ).trimmed().toFloat(), 
                    parser.attribute( "y" ).trimmed().toFloat() );
        QString xu = parser.attribute( "xunits" ).trimmed();
        QString yu = parser.attribute( "yunits" ).trimmed();

        GeoDataHotSpot::Units xunits;
        GeoDataHotSpot::Units yunits;
        
        if( xu == QString( "pixels" ) ) xunits = GeoDataHotSpot::Pixels;
        else if( xu == QString( "insetPixels" ) ) xunits = GeoDataHotSpot::InsetPixels;
        else xunits = GeoDataHotSpot::Fraction;
        if( yu == QString( "pixels" ) ) yunits = GeoDataHotSpot::Pixels;
        else if( yu == QString( "insetPixels" ) ) yunits = GeoDataHotSpot::InsetPixels;
        else yunits = GeoDataHotSpot::Fraction;
        
        parentItem.nodeAs<GeoDataIconStyle>()->setHotSpot( pf, xunits, yunits );
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_hotSpot << "> containing: " << pf
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    }
    return 0;
}

}
}
