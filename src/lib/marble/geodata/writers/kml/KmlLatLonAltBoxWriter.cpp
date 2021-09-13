// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlLatLonAltBoxWriter.h"

#include "GeoDataLatLonAltBox.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlGroundOverlayWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLatLonAltBox(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataLatLonAltBoxType,
                                 kml::kmlTag_nameSpaceOgc22 ),
    new KmlLatLonAltBoxWriter);

bool KmlLatLonAltBoxWriter::write( const GeoNode *node,
				 GeoWriter& writer ) const
{
    const GeoDataLatLonAltBox *latLonAltBox = static_cast<const GeoDataLatLonAltBox*>( node );
    writer.writeStartElement(kml::kmlTag_LatLonAltBox);
    KmlObjectTagWriter::writeIdentifiers( writer, latLonAltBox );
    writer.writeTextElement( kml::kmlTag_north, QString::number(latLonAltBox->north( GeoDataCoordinates::Degree )) );
    writer.writeTextElement( kml::kmlTag_south, QString::number(latLonAltBox->south( GeoDataCoordinates::Degree )) );
    writer.writeTextElement( kml::kmlTag_east,  QString::number(latLonAltBox->east( GeoDataCoordinates::Degree )) );
    writer.writeTextElement( kml::kmlTag_west,  QString::number(latLonAltBox->west( GeoDataCoordinates::Degree )) );
    writer.writeTextElement( kml::kmlTag_minAltitude, QString::number(latLonAltBox->minAltitude()) );
    writer.writeTextElement( kml::kmlTag_maxAltitude, QString::number(latLonAltBox->maxAltitude()) );
    KmlGroundOverlayWriter::writeAltitudeMode( writer, latLonAltBox->altitudeMode());
    writer.writeEndElement();
    return true;
}

}
