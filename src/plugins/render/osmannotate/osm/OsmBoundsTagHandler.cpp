//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "OsmBoundsTagHandler.h"

#include "GeoDataDocument.h"
#include "GeoDataLineString.h"
#include "GeoDataParser.h"
#include "GeoDataPlacemark.h"
#include "OsmBoundsGraphicsItem.h"
#include "OsmElementDictionary.h"

namespace Marble
{
namespace osm
{

static GeoTagHandlerRegistrar osmBoundsTagHandler( GeoTagHandler::QualifiedName(osmTag_bounds, ""),
                                               new OsmBoundsTagHandler() );

GeoNode* OsmBoundsTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() );

    OsmBoundsGraphicsItem* item = new OsmBoundsGraphicsItem();

    qreal minlon = parser.attribute("minlon").toDouble();
    qreal minlat = parser.attribute("minlat").toDouble();

    qreal maxlon = parser.attribute("maxlon").toDouble();
    qreal maxlat = parser.attribute("maxlat").toDouble();

    GeoDataCoordinates topLeft( minlon, minlat, 0, GeoDataCoordinates::Degree );
    GeoDataCoordinates topRight( maxlon, minlat, 0 , GeoDataCoordinates::Degree );
    GeoDataCoordinates bottomLeft( minlon, maxlat, 0 , GeoDataCoordinates::Degree );
    GeoDataCoordinates bottomRight( maxlon, maxlat, 0 , GeoDataCoordinates::Degree );

    item->append( topLeft );
    item->append( topRight );
    item->append( bottomRight );
    item->append( bottomLeft );

    QList<GeoGraphicsItem*>* model = parser.activeModel();



//
//    GeoDataPlacemark* place = new GeoDataPlacemark;
//
//    place->setCoordinate( topLeft.longitude(), topLeft.latitude(), topLeft.altitude() );
//
//    place->setGeometry( GeoDataLinearRing( bound ) );
//
//    model->append( dynamic_cast<GeoGraphicsItem*>( place ) );

    model->append( item );

    return 0;

}

}
}
