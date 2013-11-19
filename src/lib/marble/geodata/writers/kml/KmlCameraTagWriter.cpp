//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
// Copyright 2013      Illya Kovalevskyy   <illya.kovalevskyy@gmail.com>
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

    if (camera->timeStamp().when().isValid()) {
        writer.writeStartElement("gx:TimeStamp");
        writer.writeElement("when", camera->timeStamp().when().toString(Qt::ISODate));
        writer.writeEndElement();
    }

    writer.writeStartElement("gx:TimeSpan");
    if (camera->timeSpan().begin().isValid())
        writer.writeElement("begin", camera->timeSpan().begin().toString(Qt::ISODate));
    if (camera->timeSpan().end().isValid())
        writer.writeElement("end", camera->timeSpan().end().toString(Qt::ISODate));
    writer.writeEndElement();

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

