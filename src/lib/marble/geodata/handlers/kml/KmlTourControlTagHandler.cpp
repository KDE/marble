//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Illya Kovalevskyy   <illya.kovalevskyy@gmail.com>
//

#include "KmlTourControlTagHandler.h"
#include "KmlPlaylistTagHandler.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoParser.h"
#include "GeoDataPlaylist.h"
#include "GeoDataTourControl.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_GX22( TourControl )

GeoNode* KmlTourControlTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_TourControl)));

    GeoStackItem parentItem = parser.parentElement();

    GeoDataTourControl *tourControl = new GeoDataTourControl;
    KmlObjectTagHandler::parseIdentifiers( parser, tourControl );

    if (parentItem.is<GeoDataPlaylist>()) {
        //parentItem.nodeAs<GeoDataPlaylist>()->append(tourControl);
        parentItem.nodeAs<GeoDataPlaylist>()->addPrimitive(tourControl);
        return tourControl;
    } else {
        delete tourControl;
    }

    return 0;
}

} // namespace kml
} // namespace Marble
