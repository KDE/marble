//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Thibaut Gridel <tgridel@free.fr>

#include "KmlMultiTrackTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataPlacemark.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataMultiTrack.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER_GX22( MultiTrack )

GeoNode* KmlMultiTrackTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_MultiTrack)));

    GeoStackItem parentItem = parser.parentElement();

    GeoDataMultiTrack *geom = new GeoDataMultiTrack;
    KmlObjectTagHandler::parseIdentifiers( parser, geom );
    if( parentItem.represents( kmlTag_Placemark ) ) {
        parentItem.nodeAs<GeoDataPlacemark>()->setGeometry( geom );
        return parentItem.nodeAs<GeoDataPlacemark>()->geometry();

    } else if( parentItem.represents( kmlTag_MultiGeometry ) ) {
        parentItem.nodeAs<GeoDataMultiGeometry>()->append( geom );
        return geom;
    } else {
        delete geom;
        return 0;
    }
}

}
}
