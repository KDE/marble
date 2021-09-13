// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "KmlTrackTagHandler.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataMultiTrack.h"
#include "GeoDataTrack.h"
#include "GeoDataPlacemark.h"
#include "GeoDataMultiGeometry.h"
#include "KmlElementDictionary.h"
#include "MarbleDebug.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER_GX22( Track )

GeoNode *KmlTrackTagHandler::parse( GeoParser &parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Track)));

    GeoStackItem parentItem = parser.parentElement();
    GeoDataTrack *track = new GeoDataTrack();
    KmlObjectTagHandler::parseIdentifiers( parser, track );

    if ( parentItem.represents( kmlTag_Placemark ) ) {
        parentItem.nodeAs<GeoDataPlacemark>()->setGeometry( track );
        return track;
    } else if ( parentItem.represents( kmlTag_MultiTrack ) ) {
        parentItem.nodeAs<GeoDataMultiTrack>()->append( track );
        return track;
    } else if ( parentItem.represents( kmlTag_MultiGeometry ) ) {
        parentItem.nodeAs<GeoDataMultiGeometry>()->append( track );
        return track;
    } else {
        delete track;
    }

    return nullptr;
}

}
}
