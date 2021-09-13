/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
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

    return nullptr;
}

}
}
