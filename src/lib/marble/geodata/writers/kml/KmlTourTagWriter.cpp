//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013       Adrian Draghici <draghici.adrian.b@gmail.com>
//

#include "KmlTourTagWriter.h"

#include "GeoDataTour.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerTour(
        GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataTourType,
                                     kml::kmlTag_nameSpaceGx22 ),
        new KmlTourTagWriter );

bool KmlTourTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{

    const GeoDataTour *tour = static_cast<const GeoDataTour*>( node );

    writer.writeStartElement( kml::kmlTag_Tour );
    if (!tour->id().isEmpty() ) {
        writer.writeAttribute( "id", tour->id() );
    }

    writer.writeOptionalElement( kml::kmlTag_name, tour->name() );

    writer.writeOptionalElement( kml::kmlTag_description, tour->description() );

    if ( tour->playlist() != 0 ) {
        writeElement( tour->playlist(), writer );
    }

    writer.writeEndElement();

    return true;
}

}
