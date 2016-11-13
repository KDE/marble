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
            if (osmData.nodeReference(*it).isNull()) {
                osmData.nodeReference(*it).setId(--m_minId);
            }
        }
    }

    // Assigning osmData to each of the line's nodes ( if they don't already have data )
    if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLinearRingType ) {
        const GeoDataLinearRing* lineString = static_cast<GeoDataLinearRing*>( placemark->geometry() );
        for (auto it =lineString->constBegin(), end = lineString->constEnd(); it != end; ++it ) {
            if (osmData.nodeReference(*it).isNull()) {
                osmData.nodeReference(*it).setId(--m_minId);
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
        OsmPlacemarkData &outerBoundaryData = osmData.memberReference( index );
        if (outerBoundaryData.isNull()) {
            outerBoundaryData.setId(--m_minId);
        }

        // Outer boundary nodes
        QVector<GeoDataCoordinates>::const_iterator it =  outerBoundary.constBegin();
        QVector<GeoDataCoordinates>::ConstIterator const end = outerBoundary.constEnd();

        for ( ; it != end; ++it ) {
            if (outerBoundaryData.nodeReference(*it).isNull()) {
                outerBoundaryData.nodeReference(*it).setId(--m_minId);
            }
        }

        // Each inner boundary
        foreach( const GeoDataLinearRing &innerRing, polygon->innerBoundaries() ) {
            ++index;
            OsmPlacemarkData &innerRingData = osmData.memberReference( index );
            if (innerRingData.isNull()) {
                innerRingData.setId(--m_minId);
            }

            // Inner boundary nodes
            QVector<GeoDataCoordinates>::const_iterator it =  innerRing.constBegin();
            QVector<GeoDataCoordinates>::ConstIterator const end = innerRing.constEnd();

            for ( ; it != end; ++it ) {
                if (innerRingData.nodeReference(*it).isNull()) {
                    innerRingData.nodeReference(*it).setId(--m_minId);
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

