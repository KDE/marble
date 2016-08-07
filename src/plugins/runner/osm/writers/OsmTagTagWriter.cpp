//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

//Self
#include "OsmTagTagWriter.h"

//Marble
#include "GeoWriter.h"
#include "OsmElementDictionary.h"
#include "osm/OsmPlacemarkData.h"

namespace Marble
{

void OsmTagTagWriter::writeTags( const OsmPlacemarkData& osmData, GeoWriter &writer )
{
    auto it = osmData.tagsBegin();
    auto end = osmData.tagsEnd();

    for ( ; it != end; ++it ) {
        writer.writeStartElement( osm::osmTag_tag );
        writer.writeAttribute( "k", it.key() );
        writer.writeAttribute( "v", it.value() );
        writer.writeEndElement();
    }
}

}
