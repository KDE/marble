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
#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"
#include "GeoDataTypes.h"
#include "osm/OsmPlacemarkData.h"

namespace Marble {

qint64 OsmObjectManager::m_minId = -1;

void OsmObjectManager::initializeOsmData( GeoDataPlacemark* placemark )
{
    OsmPlacemarkData &osmData = placemark->osmData();

    bool isNull = osmData.isNull();
    if ( isNull ) {
        // The "--m_minId" assignments mean: assigning an id lower( by 1 ) than the current lowest,
        // and updating the current lowest id.
        osmData.setId( --m_minId );
    }

    // Assigning osmData to each of the line's nodes ( if they don't already have data )
    if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType ) {
        const GeoDataLineString* lineString = static_cast<GeoDataLineString*>( placemark->geometry() );
        QVector<GeoDataCoordinates>::const_iterator it =  lineString->constBegin();
        QVector<GeoDataCoordinates>::ConstIterator const end = lineString->constEnd();

        for ( ; it != end; ++it ) {
            if ( !osmData.containsNodeReference( *it ) ) {
                OsmPlacemarkData osmNdData;
                osmNdData.setId( --m_minId );
                osmData.addNodeReference( *it, osmNdData );
            }
        }
    }

    // Assigning osmData to each of the line's nodes ( if they don't already have data )
    if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLinearRingType ) {
        const GeoDataLinearRing* lineString = static_cast<GeoDataLinearRing*>( placemark->geometry() );
        for (auto it =lineString->constBegin(), end = lineString->constEnd(); it != end; ++it ) {
            if ( !osmData.containsNodeReference( *it ) ) {
                OsmPlacemarkData osmNdData;
                osmNdData.setId( --m_minId );
                osmData.addNodeReference( *it, osmNdData );
            }
        }
    }

    // Assigning osmData to each of the polygons boundaries, and to each of the
    // nodes that are part of those boundaries ( if they don't already have data )
    if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
        const GeoDataPolygon* polygon = static_cast<GeoDataPolygon*>( placemark->geometry() );
        const GeoDataLinearRing &outerBoundary = polygon->outerBoundary();
        int index = -1;
        if ( isNull ) {
            osmData.addTag(QStringLiteral("type"), QStringLiteral("multipolygon"));
        }

        // Outer boundary
        if ( !osmData.containsMemberReference( index ) ) {
            OsmPlacemarkData outerBoundaryData;
            outerBoundaryData.setId( --m_minId );
            osmData.addMemberReference( index, outerBoundaryData );
        }

        // Outer boundary nodes
        OsmPlacemarkData &outerBoundaryData = osmData.memberReference( index );
        QVector<GeoDataCoordinates>::const_iterator it =  outerBoundary.constBegin();
        QVector<GeoDataCoordinates>::ConstIterator const end = outerBoundary.constEnd();

        for ( ; it != end; ++it ) {
            if ( !osmData.memberReference( index ).containsNodeReference( *it ) ) {
                OsmPlacemarkData osmNodeData;
                osmNodeData.setId( --m_minId );
                outerBoundaryData.addNodeReference( *it, osmNodeData );
            }
        }

        // Each inner boundary
        foreach( const GeoDataLinearRing &innerRing, polygon->innerBoundaries() ) {
            ++index;
            if ( !osmData.containsMemberReference( index ) ) {
                OsmPlacemarkData innerRingData;
                innerRingData.setId( --m_minId );
                osmData.addMemberReference( index, innerRingData );
            }

            // Inner boundary nodes
            OsmPlacemarkData &innerRingData = osmData.memberReference( index );
            QVector<GeoDataCoordinates>::const_iterator it =  innerRing.constBegin();
            QVector<GeoDataCoordinates>::ConstIterator const end = innerRing.constEnd();

            for ( ; it != end; ++it ) {
                if ( !osmData.memberReference( index ).containsNodeReference( *it ) ) {
                    OsmPlacemarkData osmNodeData;
                    osmNodeData.setId( --m_minId );
                    innerRingData.addNodeReference( *it , osmNodeData );
                }
            }
        }
    }
}

void OsmObjectManager::registerId( qint64 id )
{
    m_minId = qMin( id, m_minId );
}

}

