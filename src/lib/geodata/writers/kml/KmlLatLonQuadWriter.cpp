//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlLatLonQuadWriter.h"

#include "GeoDataLatLonQuad.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

bool KmlLatLonQuadWriter::write( const GeoNode *node,
                 GeoWriter& writer ) const
{
    const GeoDataLatLonQuad *latLonQuad =
    static_cast<const GeoDataLatLonQuad*>( node );

    writer.writeStartElement( kml::kmlTag_nameSpaceGx22,  kml::kmlTag_LatLonQuad );

    writer.writeStartElement( kml::kmlTag_coordinates );
    QString coordinatesString = "%1,%2 %3,%4 %5,%6 %7,%8 ";
    coordinatesString += coordinatesString.arg(latLonQuad->bottomLeftLongitude(GeoDataCoordinates::Degree)).arg(latLonQuad->bottomLeftLatitude(GeoDataCoordinates::Degree));
    coordinatesString += coordinatesString.arg(latLonQuad->bottomRightLongitude(GeoDataCoordinates::Degree)).arg(latLonQuad->bottomRightLatitude(GeoDataCoordinates::Degree));
    coordinatesString += coordinatesString.arg(latLonQuad->topRightLongitude(GeoDataCoordinates::Degree)).arg(latLonQuad->topRightLatitude(GeoDataCoordinates::Degree));
    coordinatesString += coordinatesString.arg(latLonQuad->topLeftLongitude(GeoDataCoordinates::Degree)).arg(latLonQuad->topLeftLatitude(GeoDataCoordinates::Degree));

    writer.writeCharacters(coordinatesString);
    writer.writeEndElement();

    writer.writeEndElement();

    return true;
}

}



