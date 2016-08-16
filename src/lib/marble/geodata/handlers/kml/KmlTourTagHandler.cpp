//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "KmlTourTagHandler.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataTour.h"
#include "GeoDataDocument.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_GX22( Tour )

GeoNode* KmlTourTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Tour)));

    GeoStackItem parentItem = parser.parentElement();

    GeoDataTour *tour = new GeoDataTour;
    KmlObjectTagHandler::parseIdentifiers( parser, tour );

    if (parentItem.represents(kmlTag_Folder) || parentItem.represents(kmlTag_Document)) {
        parentItem.nodeAs<GeoDataContainer>()->append(tour);
        return tour;
    } else if ( parentItem.qualifiedName().first == kmlTag_kml ) {
        GeoDataDocument* doc = geoDataDoc(parser);
        doc->append(tour);
        return tour;
    } else {
        delete tour;
        return 0;
    }

    return 0;
}

} // namespace kml
} // namespace Marble
