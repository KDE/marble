/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_hotSpot)));

    GeoStackItem parentItem = parser.parentElement();
    
    if ( parentItem.represents( kmlTag_IconStyle ) ) {
        QPointF pf( parser.attribute( "x" ).trimmed().toFloat(), 
                    parser.attribute( "y" ).trimmed().toFloat() );
        QString xu = parser.attribute( "xunits" ).trimmed();
        QString yu = parser.attribute( "yunits" ).trimmed();

        GeoDataHotSpot::Units xunits;
        GeoDataHotSpot::Units yunits;

        if (xu == QLatin1String("pixels")) {
            xunits = GeoDataHotSpot::Pixels;
        } else if (xu == QLatin1String("insetPixels")) {
            xunits = GeoDataHotSpot::InsetPixels;
        } else {
            xunits = GeoDataHotSpot::Fraction;
        }

        if (yu == QLatin1String("pixels")) {
            yunits = GeoDataHotSpot::Pixels;
        } else if (yu == QLatin1String("insetPixels")) {
            yunits = GeoDataHotSpot::InsetPixels;
        } else {
            yunits = GeoDataHotSpot::Fraction;
        }

        parentItem.nodeAs<GeoDataIconStyle>()->setHotSpot( pf, xunits, yunits );
    }
    return nullptr;
}

}
}
