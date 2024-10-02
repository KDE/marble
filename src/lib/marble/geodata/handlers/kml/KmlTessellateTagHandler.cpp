/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlTessellateTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataGeometry.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(tessellate)

GeoNode *KmltessellateTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_tessellate)));

    GeoStackItem parentItem = parser.parentElement();

    QString content = parser.readElementText().trimmed();

    if (parentItem.is<GeoDataLineString>()) {
        auto lineString = parentItem.nodeAs<GeoDataLineString>();

        const bool tesselate = (content == QLatin1StringView("1"));
        lineString->setTessellate(tesselate);

    } else if (parentItem.is<GeoDataLinearRing>()) {
        auto linearRing = parentItem.nodeAs<GeoDataLinearRing>();

        const bool tesselate = (content == QLatin1StringView("1"));
        linearRing->setTessellate(tesselate);

    } else if (parentItem.is<GeoDataPolygon>()) {
        auto polygon = parentItem.nodeAs<GeoDataPolygon>();

        const bool tesselate = (content == QLatin1StringView("1"));
        polygon->setTessellate(tesselate);
    }

    return nullptr;
}

}
}
