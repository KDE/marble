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
#include "GeoDataTimeSpan.h"
#include "GeoDataTimeStamp.h"
#include "GeoWriter.h"
#include "KmlGroundOverlayWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

#include <QDateTime>

namespace Marble
{

static GeoTagWriterRegistrar s_writerCamera( GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataCameraType,
                                                                            kml::kmlTag_nameSpaceOgc22),
                                               new KmlCameraTagWriter() );


bool KmlCameraTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataCamera *camera = static_cast<const GeoDataCamera*>(node);

    writer.writeStartElement( kml::kmlTag_Camera );
    KmlObjectTagWriter::writeIdentifiers( writer, camera );

    if (camera->timeStamp().when().isValid()) {
        writer.writeStartElement("gx:TimeStamp");
        writer.writeElement("when", camera->timeStamp().when().toString(Qt::ISODate));
        writer.writeEndElement();
    }

    if (camera->timeSpan().isValid()) {
        writer.writeStartElement("gx:TimeSpan");
        if (camera->timeSpan().begin().when().isValid())
            writer.writeElement("begin", camera->timeSpan().begin().when().toString(Qt::ISODate));
        if (camera->timeSpan().end().when().isValid())
            writer.writeElement("end", camera->timeSpan().end().when().toString(Qt::ISODate));
        writer.writeEndElement();
    }

    if ( camera->longitude() != 0.0 ) {
        writer.writeElement( "longitude", QString::number( camera->longitude( GeoDataCoordinates::Degree ), 'f', 10 ) );
    }
    if ( camera->latitude() != 0.0 ) {
        writer.writeElement( "latitude", QString::number( camera->latitude( GeoDataCoordinates::Degree ), 'f', 10 ) );
    }
    writer.writeOptionalElement( "altitude", camera->altitude() );
    writer.writeOptionalElement( "roll", camera->roll() );
    writer.writeOptionalElement( "tilt", camera->tilt() );
    writer.writeOptionalElement( "heading", camera->heading() );
    KmlGroundOverlayWriter::writeAltitudeMode( writer, camera->altitudeMode() );

    writer.writeEndElement();

    return true;
}

}

