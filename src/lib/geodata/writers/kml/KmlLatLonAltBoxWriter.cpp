//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlLatLonAltBoxWriter.h"

#include "GeoDataLatLonAltBox.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "GeoTagWriter.h"
#include "KmlGroundOverlayWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

bool KmlLatLonAltBoxWriter::write( const GeoNode *node,
				 GeoWriter& writer ) const
{
    const GeoDataLatLonAltBox *latLonAltBox = static_cast<const GeoDataLatLonAltBox*>( node );
    writer.writeStartElement(kml::kmlTag_LatLonAltBox);
    writer.writeTextElement( "north", QString::number(latLonAltBox->north()) );
    writer.writeTextElement( "south", QString::number(latLonAltBox->south()) );
    writer.writeTextElement( "east",  QString::number(latLonAltBox->east()) );
    writer.writeTextElement( "west",  QString::number(latLonAltBox->west()) );
    writer.writeTextElement( "minAltitude", QString::number(latLonAltBox->minAltitude()) );
    writer.writeTextElement( "maxAltitude", QString::number(latLonAltBox->maxAltitude()) );
    QString const altitudeMode = KmlGroundOverlayWriter::altitudeModeToString( latLonAltBox->altitudeMode() );
    writer.writeTextElement( kml::kmlTag_altitudeMode, altitudeMode );
    writer.writeEndElement();
    return true;
}

}
