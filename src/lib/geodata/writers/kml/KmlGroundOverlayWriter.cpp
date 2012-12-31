//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Shou Ya <shouyatf@gmail.com>
//

#include "KmlGroundOverlayWriter.h"

#include "GeoDataGroundOverlay.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataGroundOverlayType,
				 kml::kmlTag_nameSpace22 ),
    new KmlGroundOverlayWriter );

QString KmlGroundOverlayWriter::altitudeModeToString(AltitudeMode mode)
{
    switch (mode) {
    case ClampToGround:
	return "ClampToGround";
    case RelativeToGround:
	return "RelativeToGround";
    case Absolute:
	return "Absolute";
    }
    return "";
}

bool KmlGroundOverlayWriter::write( const GeoNode *node,
				    GeoWriter& writer ) const
{
    const GeoDataGroundOverlay *ground_overlay =
	static_cast<const GeoDataGroundOverlay*>( node );

    writer.writeStartElement( kml::kmlTag_GroundOverlay );

    writer.writeTextElement( "altitude",
			     QString::number(ground_overlay->altitude()) );
    writer.writeTextElement( "altitudeMode",
			     altitudeModeToString(ground_overlay->altitudeMode()) );

    writer.writeStartElement( "LatLonBox" );
    writeElement( &ground_overlay->latLonBox(), writer );
    writer.writeEndElement();


    writer.writeEndElement();

    return true;
}

}

