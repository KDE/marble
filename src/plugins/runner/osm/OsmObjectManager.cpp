//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

// Self
#include "OsmObjectManager.h"

// Marble
#include "GeoDataPlacemark.h"
#include "GeoDataTypes.h"
#include "osm/OsmPlacemarkData.h"

namespace Marble {

qint64 OsmObjectManager::m_minId = -1;

void OsmObjectManager::initializeOsmData( GeoDataPlacemark* placemark )
{
    if ( placemark->hasOsmData() ) {
        return;
    }

    // The "--m_minId" assignments mean: assigning an id lower( by 1 ) than the current lowest,
    // and updating the current lowest id.
    OsmPlacemarkData &osmData = placemark->osmData();
    osmData.setId( --m_minId );
    osmData.setAction( "modify" );
    osmData.setVisible( "true" );

    if ( !placemark->name().isEmpty() ) {
        osmData.addTag( "name", placemark->name() );
    }

    // Assigning osmData to each of the line's nodes
    if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType ) {
        const GeoDataLineString* lineString = static_cast<GeoDataLineString*>( placemark->geometry() );
        QVector<GeoDataCoordinates>::const_iterator it =  lineString->constBegin();
        QVector<GeoDataCoordinates>::ConstIterator const end = lineString->constEnd();

        for ( ; it != end; ++it ) {
            OsmPlacemarkData osmNdData;
            osmNdData.setId( --m_minId );
            osmNdData.setAction( "modify" );
            osmNdData.setVisible( "false" );
            osmData.addReference( *it, osmNdData );
        }
    }

    // Assigning osmData to each of the polygons boundaries, and to each of the
    // nodes that are part of those boundaries
    if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
        const GeoDataPolygon* polygon = static_cast<GeoDataPolygon*>( placemark->geometry() );
        const GeoDataLinearRing &outerBoundary = polygon->outerBoundary();
        osmData.addTag( "type", "multipolygon" );

        // Outer boundary
        OsmPlacemarkData outerBoundaryData;
        outerBoundaryData.setId( --m_minId );
        outerBoundaryData.setAction( "modify" );
        outerBoundaryData.setVisible( "false" );

        QVector<GeoDataCoordinates>::const_iterator it =  outerBoundary.constBegin();
        QVector<GeoDataCoordinates>::ConstIterator const end = outerBoundary.constEnd();

        // Outer boundary nodes
        for ( ; it != end; ++it ) {
            OsmPlacemarkData osmNodeData;
            osmNodeData.setId( --m_minId );
            osmNodeData.setAction( "modify" );
            osmNodeData.setVisible( "false" );
            outerBoundaryData.addReference( *it, osmNodeData );
        }
        osmData.addReference( &outerBoundary, outerBoundaryData );

        // Each inner boundary
        foreach( const GeoDataLinearRing &innerRing, polygon->innerBoundaries() ) {
            OsmPlacemarkData innerRingData;
            innerRingData.setId( --m_minId );
            innerRingData.setAction( "modify" );
            innerRingData.setVisible( "false" );

            QVector<GeoDataCoordinates>::const_iterator it =  innerRing.constBegin();
            QVector<GeoDataCoordinates>::ConstIterator const end = innerRing.constEnd();

            // Inner boundary nodes
            for ( ; it != end; ++it ) {
                OsmPlacemarkData osmNodeData;
                osmNodeData.setId( --m_minId );
                osmNodeData.setAction( "modify" );
                osmNodeData.setVisible( "false" );
                innerRingData.addReference( *it , osmNodeData );
            }

            osmData.addReference( &innerRing, innerRingData );
        }
    }
}

void OsmObjectManager::registerId( qint64 id )
{
    m_minId = qMin( id, m_minId );
}

}

