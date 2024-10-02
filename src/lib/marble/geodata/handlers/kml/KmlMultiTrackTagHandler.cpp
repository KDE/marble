// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Thibaut Gridel <tgridel@free.fr>

#include "KmlMultiTrackTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataMultiGeometry.h"
#include "GeoDataMultiTrack.h"
#include "GeoDataPlacemark.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER_GX22(MultiTrack)

GeoNode *KmlMultiTrackTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_MultiTrack)));

    GeoStackItem parentItem = parser.parentElement();

    auto geom = new GeoDataMultiTrack;
    KmlObjectTagHandler::parseIdentifiers(parser, geom);
    if (parentItem.represents(kmlTag_Placemark)) {
        parentItem.nodeAs<GeoDataPlacemark>()->setGeometry(geom);
        return parentItem.nodeAs<GeoDataPlacemark>()->geometry();

    } else if (parentItem.represents(kmlTag_MultiGeometry)) {
        parentItem.nodeAs<GeoDataMultiGeometry>()->append(geom);
        return geom;
    } else {
        delete geom;
        return nullptr;
    }
}

}
}
