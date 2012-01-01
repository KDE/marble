//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "OsmGlobals.h"
#include "GeoDataStyle.h"
#include "GeoDataPlacemark.h"
#include "GeoDataDocument.h"
#include "GeoDataIconStyle.h"
#include "global.h"
#include "MarbleDirs.h"

namespace Marble
{
namespace osm
{
QList<QString> OsmGlobals::m_areaTags;

QColor OsmGlobals::backgroundColor( 0xF1, 0xEE, 0xE8 );
QList<GeoDataPlacemark*> OsmGlobals::dummyPlacemarks;

bool OsmGlobals::tagNeedArea(const QString& keyValue)
{
    if ( m_areaTags.count() < 1 )
        setupAreaTags();
    
    return qBinaryFind( m_areaTags.constBegin(), m_areaTags.constEnd(), keyValue ) != m_areaTags.constEnd();
}

void OsmGlobals::setupAreaTags()
{
    m_areaTags.append( "landuse=forest" );
    m_areaTags.append( "natural=wood" );
    m_areaTags.append( "area=yes" );
    m_areaTags.append( "waterway=riverbank" );
    m_areaTags.append( "building=yes" );
    m_areaTags.append( "amenity=parking" );
    m_areaTags.append( "leisure=park" );
    
    m_areaTags.append( "landuse=allotments" );
    m_areaTags.append( "landuse=basin" );
    m_areaTags.append( "landuse=brownfield" );
    m_areaTags.append( "landuse=cemetery" );
    m_areaTags.append( "landuse=commercial" );
    m_areaTags.append( "landuse=construction" );
    m_areaTags.append( "landuse=farm" );
    m_areaTags.append( "landuse=farmland" );
    m_areaTags.append( "landuse=farmyard" );
    m_areaTags.append( "landuse=garages" );
    m_areaTags.append( "landuse=greenfield" );
    m_areaTags.append( "landuse=industrial" );
    m_areaTags.append( "landuse=landfill" );
    m_areaTags.append( "landuse=meadow" );
    m_areaTags.append( "landuse=military" );
    m_areaTags.append( "landuse=orchard" );
    m_areaTags.append( "landuse=quarry" );
    m_areaTags.append( "landuse=railway" );
    m_areaTags.append( "landuse=reservoir" );
    m_areaTags.append( "landuse=residential" );
    m_areaTags.append( "landuse=retail" );
    
    qSort( m_areaTags.begin(), m_areaTags.end() );
}

void OsmGlobals::addDummyPlacemark( GeoDataPlacemark* placemark )
{
    dummyPlacemarks << placemark;
}

void OsmGlobals::cleanUpDummyPlacemarks()
{
    foreach( GeoDataFeature* placemark, dummyPlacemarks )
    {
        delete placemark;
    }
    dummyPlacemarks.clear();
}

}
}

