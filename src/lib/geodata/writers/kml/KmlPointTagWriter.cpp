//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "KmlPointTagWriter.h"

#include "GeoDataPoint.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerPoint( GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataPointType,
                                                                            kml::kmlTag_nameSpace22),
                                               new KmlPointTagWriter() );


bool KmlPointTagWriter::write( const GeoDataObject &node,
                               GeoWriter& writer ) const
{
    const GeoDataPoint &point = static_cast<const GeoDataPoint&>(node);

    writer.writeStartElement( kml::kmlTag_Point );

    writer.writeStartElement("coordinates");

    QString coordinateString;

    //FIXME: this should be using the GeoDataCoordinates::toString but currently
    // it is not including the altitude and is adding an extra space after commas

    coordinateString += QString::number( point.longitude( GeoDataCoordinates::Degree ), 'f', 10 );
    coordinateString += ',' ;
    coordinateString += QString::number( point.latitude( GeoDataCoordinates::Degree ) , 'f', 10 );

    if( point.altitude() ) {
        coordinateString += ',';
        coordinateString += QString::number( point.altitude() , 'f' , 10);
    }

    writer.writeCharacters( coordinateString );

    writer.writeEndElement();
    writer.writeEndElement();

    return true;
}

}
