// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "KmlTourTagHandler.h"
#include "GeoDataDocument.h"
#include "GeoDataParser.h"
#include "GeoDataTour.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_GX22(Tour)

GeoNode *KmlTourTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Tour)));

    GeoStackItem parentItem = parser.parentElement();

    auto tour = new GeoDataTour;
    KmlObjectTagHandler::parseIdentifiers(parser, tour);

    if (parentItem.represents(kmlTag_Folder) || parentItem.represents(kmlTag_Document)) {
        parentItem.nodeAs<GeoDataContainer>()->append(tour);
        return tour;
    } else if (parentItem.qualifiedName().first == QString::fromLatin1(kmlTag_kml)) {
        GeoDataDocument *doc = geoDataDoc(parser);
        doc->append(tour);
        return tour;
    } else {
        delete tour;
        return nullptr;
    }

    return nullptr;
}

} // namespace kml
} // namespace Marble
