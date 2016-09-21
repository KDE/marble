//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015    Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//


#include "KmlOsmPlacemarkDataTagWriter.h"

#include "GeoDataPlacemark.h"
#include "GeoDataSchemaData.h"
#include "GeoDataExtendedData.h"
#include "GeoDataFeature.h"
#include "GeoDataTypes.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "osm/OsmPlacemarkData.h"

namespace Marble
{

bool KmlOsmPlacemarkDataTagWriter::write( const GeoDataFeature *feature,
                                          GeoWriter& writer )
{
    if ( feature->nodeType() != GeoDataTypes::GeoDataPlacemarkType ) {
        return false;
    }
    const GeoDataPlacemark *placemark = static_cast<const GeoDataPlacemark*>( feature );

    writer.writeStartElement( kml::kmlTag_ExtendedData );

    // We declare the "mx" namespace for the custom osmPlacemarkData XML schema
    writer.writeNamespace( kml::kmlTag_nameSpaceMx, "mx" );

    const OsmPlacemarkData &osmData = placemark->osmData();
    KmlOsmPlacemarkDataTagWriter::writeOsmData( placemark->geometry(), osmData, writer );

    writer.writeEndElement();
    return true;
}

bool KmlOsmPlacemarkDataTagWriter::writeOsmData( const GeoDataGeometry *geometry,
                                                 const OsmPlacemarkData &osmData,
                                                 GeoWriter& writer )
{
    writer.writeStartElement( kml::kmlTag_nameSpaceMx, kml::kmlTag_OsmPlacemarkData );

    // Writing the attributes
    writer.writeAttribute( "id", QString::number( osmData.id() ) );
    writer.writeOptionalAttribute( "changeset", osmData.changeset() );
    writer.writeOptionalAttribute( "timestamp", osmData.timestamp() );
    writer.writeOptionalAttribute( "uid", osmData.uid() );
    writer.writeOptionalAttribute( "user", osmData.user() );
    writer.writeOptionalAttribute( "version", osmData.version() );
    writer.writeOptionalAttribute( "visible", osmData.isVisible() );
    writer.writeOptionalAttribute( "action", osmData.action() );

    // Writing the tags
    QHash<QString, QString>::const_iterator tagsIt = osmData.tagsBegin();
    QHash<QString, QString>::const_iterator tagsEnd = osmData.tagsEnd();
    for ( ; tagsIt != tagsEnd; ++tagsIt ) {
        writer.writeStartElement( kml::kmlTag_nameSpaceMx, "tag" );
        writer.writeAttribute( "k", tagsIt.key() );
        writer.writeAttribute( "v", tagsIt.value() );
        writer.writeEndElement();
    }
    if ( geometry ) {

        // Ways
        if ( geometry->nodeType() == GeoDataTypes::GeoDataLineStringType ||
             geometry->nodeType() == GeoDataTypes::GeoDataLinearRingType ) {
            int ndIndex = 0;

            // Writing the component nodes
            const GeoDataLineString *lineString = static_cast<const GeoDataLineString*>( geometry );
            QVector<GeoDataCoordinates>::const_iterator nodeIt = lineString->begin();
            QVector<GeoDataCoordinates>::const_iterator nodeEnd = lineString->end();
            for ( ; nodeIt != nodeEnd; ++nodeIt ) {
                const OsmPlacemarkData &nodeOsmData = osmData.nodeReference( *nodeIt );
                writer.writeStartElement( kml::kmlTag_nameSpaceMx, "nd" );
                writer.writeAttribute( "index", QString::number( ndIndex++ ) );
                writeOsmData( nullptr, nodeOsmData, writer );
                writer.writeEndElement();
            }
        }
        // Polygons
        else if ( geometry->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
            int memberIndex = -1;

            // Writing the outerBoundary osmData
            const GeoDataPolygon *polygon = static_cast<const GeoDataPolygon*>( geometry );
            const GeoDataLinearRing &outerRing = polygon->outerBoundary();
            const OsmPlacemarkData &outerRingOsmData = osmData.memberReference( memberIndex );
            writer.writeStartElement( kml::kmlTag_nameSpaceMx, kml::kmlTag_member );
            writer.writeAttribute( "index", QString::number( memberIndex ) );
            writeOsmData( &outerRing, outerRingOsmData, writer );
            writer.writeEndElement();

            // Writing the innerBoundaries
            foreach ( const GeoDataLinearRing &innerRing, polygon->innerBoundaries() ) {
                const OsmPlacemarkData &innerRingOsmData = osmData.memberReference( ++memberIndex );
                writer.writeStartElement( kml::kmlTag_nameSpaceMx, kml::kmlTag_member );
                writer.writeAttribute( "index", QString::number( memberIndex ) );
                writeOsmData( &innerRing, innerRingOsmData, writer );
                writer.writeEndElement();
            }
        }
    }
    writer.writeEndElement();
    return true;
}

}
