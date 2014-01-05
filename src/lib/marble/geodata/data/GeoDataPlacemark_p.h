//
// This file is part of the Marble Virtual Globe.
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

#include "GeoDataMultiTrack.h"
#include "GeoDataTrack.h"
#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataPlacemarkPrivate : public GeoDataFeaturePrivate
{
  public:
    GeoDataPlacemarkPrivate()
      : m_geometry( new GeoDataPoint ),
        m_area( -1.0 ),
        m_population( -1 )
    {
    }

    virtual ~GeoDataPlacemarkPrivate()
    {
        delete m_geometry;
    }

    GeoDataPlacemarkPrivate& operator=( const GeoDataPlacemarkPrivate& other )
    {
        if ( this == &other ) {
            return *this;
        }

        GeoDataFeaturePrivate::operator=( other );

        GeoDataGeometry * geometry = 0;
        if ( other.m_geometry ) {
            switch( other.m_geometry->geometryId() ) {
            case InvalidGeometryId:
                break;
            case GeoDataPointId:
                geometry = new GeoDataPoint( *static_cast<const GeoDataPoint*>( other.m_geometry ) );
                break;
            case GeoDataLineStringId:
                geometry = new GeoDataLineString( *static_cast<const GeoDataLineString*>( other.m_geometry ) );
                break;
            case GeoDataLinearRingId:
                geometry = new GeoDataLinearRing( *static_cast<const GeoDataLinearRing*>( other.m_geometry ) );
                break;
            case GeoDataPolygonId:
                geometry = new GeoDataPolygon( *static_cast<const GeoDataPolygon*>( other.m_geometry ) );
                break;
            case GeoDataMultiGeometryId:
                geometry = new GeoDataMultiGeometry( *static_cast<const GeoDataMultiGeometry*>( other.m_geometry ) );
                break;
            case GeoDataTrackId:
                geometry = new GeoDataTrack( *static_cast<const GeoDataTrack*>( other.m_geometry ) );
                break;
            case GeoDataMultiTrackId:
                geometry = new GeoDataMultiTrack( *static_cast<const GeoDataMultiTrack*>( other.m_geometry ) );
                break;
            case GeoDataModelId:
                break;
            default: break;
            }
        }
        delete m_geometry;
        m_geometry = geometry;

        m_countrycode = other.m_countrycode;
        m_area = other.m_area;
        m_population = other.m_population;
        m_state = other.m_state;

        return *this;
    }

    virtual GeoDataFeaturePrivate* copy()
    {
        GeoDataPlacemarkPrivate* copy = new GeoDataPlacemarkPrivate;
        *copy = *this;
        return copy;
    }

    virtual const char* nodeType() const
    {
        return GeoDataTypes::GeoDataPlacemarkType;
    }

    virtual EnumFeatureId featureId() const
    {
        return GeoDataPlacemarkId;
    }

    // Data for a Placemark in addition to those in GeoDataFeature.
    GeoDataGeometry    *m_geometry;     // any GeoDataGeometry entry like locations
    QString             m_countrycode;  // Country code.
    qreal               m_area;         // Area in square kilometer
    qint64              m_population;   // population in number of inhabitants
    QString             m_state;        // State
};

} // namespace Marble

#endif
