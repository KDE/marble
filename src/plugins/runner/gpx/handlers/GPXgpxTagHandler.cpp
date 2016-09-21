/*
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>

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

#include "GPXgpxTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoDataParser.h"

#include "GeoDataDocument.h"
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"
#include "GeoDataLineStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataHotSpot.h"
#include "MarbleDirs.h"
#include "MarbleGlobal.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(gpx)

GeoNode* GPXgpxTagHandler::parse(GeoParser& parser) const
{
    GeoDataDocument* doc = geoDataDoc( parser );

    GeoDataStyle::Ptr style(new GeoDataStyle);
    GeoDataLineStyle lineStyle;
    QColor transparentRed = Oxygen::brickRed6;
    transparentRed.setAlpha( 200 );
    lineStyle.setColor( transparentRed );
    lineStyle.setWidth( 4 );
    style->setLineStyle(lineStyle);
    style->setId(QStringLiteral("track"));

    GeoDataStyleMap styleMap;
    styleMap.setId(QStringLiteral("map-track"));
    styleMap.insert(QStringLiteral("normal"), QLatin1Char('#') + style->id());
    doc->addStyleMap(styleMap);
    doc->addStyle(style);

    // create a style for routes
    GeoDataStyle::Ptr routestyle(new GeoDataStyle);
    GeoDataLineStyle routeLineStyle;
    QColor skyBlue = Oxygen::skyBlue6;
    skyBlue.setAlpha( 200 );
    routeLineStyle.setColor( skyBlue );
    routeLineStyle.setWidth( 5 );
    routestyle->setLineStyle(routeLineStyle);
    routestyle->setId(QStringLiteral("route"));

    GeoDataStyleMap routeStyleMap;
    routeStyleMap.setId(QStringLiteral("map-route"));
    routeStyleMap.insert(QStringLiteral("normal"), QLatin1Char('#') + routestyle->id());
    doc->addStyleMap(routeStyleMap);
    doc->addStyle(routestyle);

    // create a default style for waypoint icons
    GeoDataStyle::Ptr waypointStyle(new GeoDataStyle);
    waypointStyle->setId(QStringLiteral("waypoint"));
    GeoDataIconStyle iconStyle;
    iconStyle.setIconPath(MarbleDirs::path(QStringLiteral("bitmaps/flag.png")));
    iconStyle.setHotSpot(QPointF(0.12,0.03), GeoDataHotSpot::Fraction, GeoDataHotSpot::Fraction);
    waypointStyle->setIconStyle(iconStyle);

    GeoDataLabelStyle waypointLabelStyle;
    waypointLabelStyle.setAlignment(GeoDataLabelStyle::Corner);
    waypointStyle->setLabelStyle(waypointLabelStyle);

    GeoDataStyleMap waypointStyleMap;
    waypointStyleMap.setId(QStringLiteral("map-waypoint"));
    waypointStyleMap.insert(QStringLiteral("normal"), QLatin1Char('#') + waypointStyle->id());
    doc->addStyleMap(waypointStyleMap);
    doc->addStyle(waypointStyle);

    return doc;
}

}
}
