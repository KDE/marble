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

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataLatLonBoxType,
				 kml::kmlTag_nameSpace22 ),
    new KmlLatLonBoxWriter );

bool KmlLatLonBoxWriter::write( const GeoNode *node,
				 GeoWriter& writer ) const
{
    const GeoDataLatLonBox *lat_lon_box =
	static_cast<const GeoDataLatLonBox*>( node );

    writer.writeStartElement( kml::kmlTag_LatLonBox );

    writer.writeTextElement( "north",
			     QString::number(lat_lon_box->north()) );
    writer.writeTextElement( "south",
			     QString::number(lat_lon_box->south()) );
    writer.writeTextElement( "east",
			     QString::number(lat_lon_box->east()) );
    writer.writeTextElement( "west",
			     QString::number(lat_lon_box->west()) );
    writer.writeTextElement( "rotation",
			     QString::number(lat_lon_box->rotation()) );

    writer.writeEndElement();

    return true;
}

}


