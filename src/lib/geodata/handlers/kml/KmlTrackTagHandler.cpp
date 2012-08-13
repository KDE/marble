//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "KmlTrackTagHandler.h"

#include "GeoDataMultiTrack.h"
#include "GeoDataTrack.h"
#include "GeoDataPlacemark.h"
#include "KmlElementDictionary.h"
#include "MarbleDebug.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER_GX22( Track )

GeoNode *KmlTrackTagHandler::parse( GeoParser &parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_Track ) );

    GeoStackItem parentItem = parser.parentElement();

    #ifdef DEBUG_TAGS
    mDebug() << "Parsed <" << kmlTag_Track << ">"
             << " parent item name: " << parentItem.qualifiedName().first;
    #endif // DEBUG_TAGS

    if ( parentItem.represents( kmlTag_Placemark ) ) {
        GeoDataTrack *track = new GeoDataTrack();
        parentItem.nodeAs<GeoDataPlacemark>()->setGeometry( track );
        return track;
    } else if ( parentItem.represents( kmlTag_MultiTrack ) ) {
        GeoDataTrack *track = new GeoDataTrack();
        parentItem.nodeAs<GeoDataMultiTrack>()->append( track );
        return track;
    } else if ( parentItem.represents( kmlTag_MultiGeometry ) ) {
        GeoDataTrack *track = new GeoDataTrack();
        parentItem.nodeAs<GeoDataMultiGeometry>()->append( track );
        return track;
    }

    return 0;
}

}
}
