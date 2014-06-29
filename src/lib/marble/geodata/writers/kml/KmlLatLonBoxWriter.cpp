//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Shou Ya <shouyatf@gmail.com>
//

#include "KmlLatLonBoxWriter.h"

#include "GeoDataLatLonBox.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataLatLonBoxType,
				 kml::kmlTag_nameSpaceOgc22 ),
    new KmlLatLonBoxWriter );

bool KmlLatLonBoxWriter::write( const GeoNode *node,
				 GeoWriter& writer ) const
{
    const GeoDataLatLonBox *lat_lon_box =
	static_cast<const GeoDataLatLonBox*>( node );

    writer.writeStartElement( kml::kmlTag_LatLonBox );
    KmlObjectTagWriter::writeIdentifiers( writer, lat_lon_box );

    writer.writeTextElement( "north",
			     QString::number(lat_lon_box->north( GeoDataCoordinates::Degree )) );
    writer.writeTextElement( "south",
			     QString::number(lat_lon_box->south( GeoDataCoordinates::Degree )) );
    writer.writeTextElement( "east",
			     QString::number(lat_lon_box->east( GeoDataCoordinates::Degree )) );
    writer.writeTextElement( "west",
			     QString::number(lat_lon_box->west( GeoDataCoordinates::Degree )) );
    writer.writeOptionalElement( "rotation",
                             QString::number(lat_lon_box->rotation( GeoDataCoordinates::Degree )), "0" );

    writer.writeEndElement();

    return true;
}

}


