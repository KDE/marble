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

    GeoDataStyle style;
    GeoDataLineStyle lineStyle;
    lineStyle.setColor( Oxygen::forestGreen4 );
    lineStyle.setWidth(2);
    style.setLineStyle(lineStyle);
    style.setStyleId("track");

    GeoDataStyleMap styleMap;
    styleMap.setStyleId("map-track");
    styleMap.insert("normal", QString("#").append(style.styleId()));
    doc->addStyleMap(styleMap);
    doc->addStyle(style);

    // create a style for routes
    GeoDataStyle routestyle;
    GeoDataLineStyle routeLineStyle;
    routeLineStyle.setColor( Oxygen::skyBlue4 );
    routeLineStyle.setWidth(2);
    routestyle.setLineStyle(routeLineStyle);
    routestyle.setStyleId("route");

    GeoDataStyleMap routeStyleMap;
    routeStyleMap.setStyleId("map-route");
    routeStyleMap.insert("normal", QString("#").append(routestyle.styleId()));
    doc->addStyleMap(routeStyleMap);
    doc->addStyle(routestyle);

    // create a default style for waypoint icons
    GeoDataStyle waypointStyle;
    waypointStyle.setStyleId("waypoint");
    GeoDataIconStyle iconStyle;
    iconStyle.setIconPath(MarbleDirs::path("bitmaps/flag.png"));
    iconStyle.setHotSpot(QPointF(0.12,0.03), GeoDataHotSpot::Fraction, GeoDataHotSpot::Fraction);
    waypointStyle.setIconStyle(iconStyle);

    GeoDataLabelStyle waypointLabelStyle;
    waypointLabelStyle.setAlignment(GeoDataLabelStyle::Corner);
    waypointStyle.setLabelStyle(waypointLabelStyle);

    GeoDataStyleMap waypointStyleMap;
    waypointStyleMap.setStyleId("map-waypoint");
    waypointStyleMap.insert("normal", QString("#").append(waypointStyle.styleId()));
    doc->addStyleMap(waypointStyleMap);
    doc->addStyle(waypointStyle);

#ifdef DEBUG_TAGS
    mDebug() << "Parsed <" << gpxTag_gpx << "> document: " << doc;
#endif
    return doc;
}

}
}
