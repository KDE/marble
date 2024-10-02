/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlHotSpotTagHandler.h"

#include "GeoDataHotSpot.h"
#include "GeoDataIconStyle.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(hotSpot)

GeoNode *KmlhotSpotTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_hotSpot)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_IconStyle)) {
        QPointF pf(parser.attribute("x").trimmed().toFloat(), parser.attribute("y").trimmed().toFloat());
        QString xu = parser.attribute("xunits").trimmed();
        QString yu = parser.attribute("yunits").trimmed();

        GeoDataHotSpot::Units xunits;
        GeoDataHotSpot::Units yunits;

        if (xu == QLatin1StringView("pixels")) {
            xunits = GeoDataHotSpot::Pixels;
        } else if (xu == QLatin1StringView("insetPixels")) {
            xunits = GeoDataHotSpot::InsetPixels;
        } else {
            xunits = GeoDataHotSpot::Fraction;
        }

        if (yu == QLatin1StringView("pixels")) {
            yunits = GeoDataHotSpot::Pixels;
        } else if (yu == QLatin1StringView("insetPixels")) {
            yunits = GeoDataHotSpot::InsetPixels;
        } else {
            yunits = GeoDataHotSpot::Fraction;
        }

        parentItem.nodeAs<GeoDataIconStyle>()->setHotSpot(pf, xunits, yunits);
    }
    return nullptr;
}

}
}
