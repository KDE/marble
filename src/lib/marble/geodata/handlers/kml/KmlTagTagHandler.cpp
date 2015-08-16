//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015    Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

#include "KmlTagTagHandler.h"

#include "KmlElementDictionary.h"
#include "osm/OsmPlacemarkData.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_MX( tag )

GeoNode* KmltagTagHandler::parse( GeoParser& parser ) const
{
    if ( parser.parentElement().is<OsmPlacemarkData>() ) {
        QString key = parser.attribute( "k" );
        QString value = parser.attribute( "v" );
        OsmPlacemarkData *osmData = parser.parentElement().nodeAs<OsmPlacemarkData>();
        osmData->addTag( key, value );
    }

    return 0;
}

}
}

