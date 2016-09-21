//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Gaurav Gupta <1989.gaurav@googlemail.com>
// Copyright 2013      Illya Kovalevskyy   <illya.kovalevskyy@gmail.com>
//


#include "KmlLookAtTagWriter.h"

#include "GeoDataLookAt.h"
#include "GeoDataTypes.h"
#include "GeoDataTimeSpan.h"
#include "GeoDataTimeStamp.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlGroundOverlayWriter.h"
#include "KmlObjectTagWriter.h"

#include <QDateTime>

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt( GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataLookAtType,
                                                                            kml::kmlTag_nameSpaceOgc22),
                                               new KmlLookAtTagWriter() );


bool KmlLookAtTagWriter::write( const GeoNode *node,
                               GeoWriter& writer ) const
{
    const GeoDataLookAt *lookAt = static_cast<const GeoDataLookAt*>(node);

    writer.writeStartElement( kml::kmlTag_LookAt );
    KmlObjectTagWriter::writeIdentifiers( writer, lookAt );

    if (lookAt->timeStamp().when().isValid()) {
        writer.writeStartElement("gx:TimeStamp");
        writer.writeElement("when", lookAt->timeStamp().when().toString(Qt::ISODate));
        writer.writeEndElement();
    }

    if( lookAt->timeSpan().isValid() ){
        writer.writeStartElement("gx:TimeSpan");
        if (lookAt->timeSpan().begin().when().isValid())
            writer.writeElement("begin", lookAt->timeSpan().begin().when().toString(Qt::ISODate));
        if (lookAt->timeSpan().end().when().isValid())
            writer.writeElement("end", lookAt->timeSpan().end().when().toString(Qt::ISODate));
        writer.writeEndElement();
    }

    writer.writeOptionalElement( "longitude", QString::number( lookAt->longitude( GeoDataCoordinates::Degree ), 'f', 10 ) );
    writer.writeOptionalElement( "latitude", QString::number( lookAt->latitude( GeoDataCoordinates::Degree ), 'f', 10 ) );
    writer.writeOptionalElement( "altitude", QString::number( lookAt->altitude(), 'f', 10 ) );
    writer.writeOptionalElement( "range", QString::number( lookAt->range(), 'f', 10 ) );
    KmlGroundOverlayWriter::writeAltitudeMode( writer, lookAt->altitudeMode() );

    writer.writeEndElement();

    return true;
}

}
