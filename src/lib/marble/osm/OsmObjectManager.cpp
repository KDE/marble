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
#include "GeoDataBuilding.h"
#include "GeoDataMultiGeometry.h"
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
    if (const auto lineString = geodata_cast<GeoDataLineString>(placemark->geometry())) {
        QVector<GeoDataCoordinates>::const_iterator it =  lineString->constBegin();
        QVector<GeoDataCoordinates>::ConstIterator const end = lineString->constEnd();

        for ( ; it != end; ++it ) {
            if (osmData.nodeReference(*it).isNull()) {
                osmData.nodeReference(*it).setId(--m_minId);
            }
        }
    }

    const auto building = geodata_cast<GeoDataBuilding>(placemark->geometry());

    GeoDataLinearRing* lineString;
    if (building) {
        lineString = geodata_cast<GeoDataLinearRing>(&building->multiGeometry()->at(0));
    } else {
        lineString = geodata_cast<GeoDataLinearRing>(placemark->geometry());
    }
    // Assigning osmData to each of the line's nodes ( if they don't already have data )
    if (lineString) {
        for (auto it =lineString->constBegin(), end = lineString->constEnd(); it != end; ++it ) {
            if (osmData.nodeReference(*it).isNull()) {
                osmData.nodeReference(*it).setId(--m_minId);
            }
        }
    }

    GeoDataPolygon* polygon;
    if (building) {
        polygon = geodata_cast<GeoDataPolygon>(&building->multiGeometry()->at(0));
    } else {
        polygon = geodata_cast<GeoDataPolygon>(placemark->geometry());
    }
    // Assigning osmData to each of the polygons boundaries, and to each of the
    // nodes that are part of those boundaries ( if they don't already have data )
    if (polygon) {
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
        for( const GeoDataLinearRing &innerRing: polygon->innerBoundaries() ) {
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

