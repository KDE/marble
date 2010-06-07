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
    lineStyle.setColor(Qt::green);
    lineStyle.setWidth(2);
    style.setLineStyle(lineStyle);
    style.setStyleId("track");

    GeoDataStyleMap *styleMap = new GeoDataStyleMap();
    styleMap->setStyleId("map-track");
    styleMap->insert("normal", QString("#").append(style.styleId()));
    doc->addStyleMap(*styleMap);
    doc->addStyle(style);

#ifdef DEBUG_TAGS
    mDebug() << "Parsed <" << gpxTag_gpx << "> document: " << doc;
#endif
    return doc;
}

}
}
