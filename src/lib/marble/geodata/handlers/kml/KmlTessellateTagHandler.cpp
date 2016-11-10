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

#include "KmlTessellateTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"
#include "GeoDataGeometry.h"
#include "GeoDataPoint.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( tessellate )

GeoNode* KmltessellateTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_tessellate)));

    GeoStackItem parentItem = parser.parentElement();
    
    QString content = parser.readElementText().trimmed();

    if( parentItem.is<GeoDataLineString>() ) {
        GeoDataLineString* lineString = parentItem.nodeAs<GeoDataLineString>();

        const bool tesselate = (content == QLatin1String("1"));
        lineString->setTessellate(tesselate);

    } else if( parentItem.is<GeoDataLinearRing>() ) {
        GeoDataLinearRing* linearRing = parentItem.nodeAs<GeoDataLinearRing>();

        const bool tesselate = (content == QLatin1String("1"));
        linearRing->setTessellate(tesselate);

    } else if( parentItem.is<GeoDataPolygon>() ) {
        GeoDataPolygon* polygon = parentItem.nodeAs<GeoDataPolygon>();

        const bool tesselate = (content == QLatin1String("1"));
        polygon->setTessellate(tesselate);
    }

    return 0;
}

}
}
