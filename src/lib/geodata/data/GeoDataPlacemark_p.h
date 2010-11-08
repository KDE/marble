//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATAPLACEMARKPRIVATE_H
#define MARBLE_GEODATAPLACEMARKPRIVATE_H

#include "GeoDataFeature_p.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataPlacemarkPrivate : public GeoDataFeaturePrivate
{
  public:
    GeoDataPlacemarkPrivate()
      : m_geometry( 0 ),
        m_area( -1.0 ),
        m_population( -1 ),
        m_lookAt( 0 )
    {
    }

    virtual ~GeoDataPlacemarkPrivate()
    {
        delete m_geometry;
        delete m_lookAt;
    }

    void operator=( const GeoDataPlacemarkPrivate& other )
    {
        GeoDataFeaturePrivate::operator=( other );
        m_coordinate = other.m_coordinate;
        m_countrycode = other.m_countrycode;
        m_area = other.m_area;
        m_population = other.m_population;
        m_state = other.m_state;

        delete m_geometry;
        if( !other.m_geometry ) return;

        switch( other.m_geometry->geometryId() ) {
            case InvalidGeometryId:
                break;
            case GeoDataPointId:
                m_geometry = new GeoDataPoint( *static_cast<GeoDataPoint*>( other.m_geometry ) );
                break;
            case GeoDataLineStringId:
                m_geometry = new GeoDataLineString( *static_cast<GeoDataLineString*>( other.m_geometry ) );
                break;
            case GeoDataLinearRingId:
                m_geometry = new GeoDataLinearRing( *static_cast<GeoDataLinearRing*>( other.m_geometry ) );
                break;
            case GeoDataPolygonId:
                m_geometry = new GeoDataPolygon( *static_cast<GeoDataPolygon*>( other.m_geometry ) );
                break;
            case GeoDataMultiGeometryId:
                m_geometry = new GeoDataMultiGeometry( *static_cast<GeoDataMultiGeometry*>( other.m_geometry ) );
                break;
            case GeoDataModelId:
                break;
            default: break;
        };
    }

    virtual void* copy()
    {
        GeoDataPlacemarkPrivate* copy = new GeoDataPlacemarkPrivate;
        *copy = *this;
        return copy;
    }

    virtual QString nodeType() const
    {
        return GeoDataTypes::GeoDataPlacemarkType;
    }

    virtual EnumFeatureId featureId() const
    {
        return GeoDataPlacemarkId;
    }

    // Data for a Placemark in addition to those in GeoDataFeature.
    GeoDataGeometry    *m_geometry;     // any GeoDataGeometry entry like locations
    GeoDataPoint        m_coordinate;   // The geographic position
    QString             m_countrycode;  // Country code.
    qreal               m_area;         // Area in square kilometer
    qint64              m_population;   // population in number of inhabitants
    QString             m_state;        // State
    GeoDataLookAt      *m_lookAt;       // lookAt
};

} // namespace Marble

#endif
