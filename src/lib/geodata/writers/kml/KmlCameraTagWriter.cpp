//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//


#include "KmlCameraTagWriter.h"

#include "GeoDataCamera.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlGroundOverlayWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

bool KmlCameraTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataCamera *camera = static_cast<const GeoDataCamera*>(node);

    writer.writeStartElement( kml::kmlTag_Camera );

    writer.writeElement( "longitude", QString::number( camera->longitude( GeoDataCoordinates::Degree ), 'f', 10 ) );
    writer.writeElement( "latitude", QString::number( camera->latitude( GeoDataCoordinates::Degree ), 'f', 10 ) );
    writer.writeElement( "altitude", QString::number( camera->altitude(), 'f', 10 ) );
    writer.writeElement( "roll", QString::number( camera->roll()) );
    writer.writeElement( "tilt", QString::number( camera->tilt()) );
    writer.writeElement( "heading", QString::number( camera->heading()) );


    writer.writeTextElement( kml::kmlTag_altitudeMode, KmlGroundOverlayWriter::altitudeModeToString(camera->altitudeMode()) );

    writer.writeEndElement();

    return true;
}

}

