//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATAMULTIGEOMETRYPRIVATE_H
#define MARBLE_GEODATAMULTIGEOMETRYPRIVATE_H

#include "GeoDataGeometry_p.h"

#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "GeoDataTrack.h"
#include "GeoDataMultiTrack.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataModel.h"
#include "GeoDataTypes.h"


namespace Marble
{

class GeoDataMultiGeometryPrivate : public GeoDataGeometryPrivate
{
  public:
    GeoDataMultiGeometryPrivate()
    {
    }

    ~GeoDataMultiGeometryPrivate()
    {
        qDeleteAll(m_vector);
    }

    GeoDataMultiGeometryPrivate& operator=( const GeoDataMultiGeometryPrivate &other)
    {
        GeoDataGeometryPrivate::operator=( other );

        qDeleteAll( m_vector );
        m_vector.clear();

        m_vector.reserve(other.m_vector.size());

        foreach( GeoDataGeometry *geometry, other.m_vector ) {
            GeoDataGeometry *newGeometry;

            // This piece of code has been used for a couple of times. Isn't it possible
            // to add a virtual method copy() similar to how abstract view does this?
            if ( geometry->nodeType() == GeoDataTypes::GeoDataLineStringType ) {
                newGeometry = new GeoDataLineString( *geometry );
            } else if ( geometry->nodeType() == GeoDataTypes::GeoDataPointType ) {
                // FIXME: Doesn't have a constructor which creates the object from a
                // GeoDataGeometry so cast is needed.
                newGeometry = new GeoDataPoint( *static_cast<GeoDataPoint*>( geometry ) );
            } else if ( geometry->nodeType() == GeoDataTypes::GeoDataModelType ) {
                // FIXME: Doesn't have a constructor which creates the object from a
                // GeoDataGeometry so cast is needed.
                newGeometry = new GeoDataModel( *static_cast<GeoDataModel*>( geometry ) );
            } else if ( geometry->nodeType() == GeoDataTypes::GeoDataTrackType ) {
                newGeometry = new GeoDataTrack( *static_cast<GeoDataTrack*>( geometry ) );
            } else if ( geometry->nodeType() == GeoDataTypes::GeoDataMultiTrackType ) {
                newGeometry = new GeoDataMultiTrack( *geometry );
            } else if ( geometry->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
                newGeometry = new GeoDataPolygon( *geometry );
            } else if ( geometry->nodeType() == GeoDataTypes::GeoDataLinearRingType ) {
                newGeometry = new GeoDataLinearRing( *geometry );
            } else {
                qFatal("Unexpected type '%s'", geometry->nodeType() );
            }

            m_vector.append( newGeometry );
        }
        return *this;
    }

    virtual GeoDataGeometryPrivate* copy()
    { 
        GeoDataMultiGeometryPrivate* copy = new GeoDataMultiGeometryPrivate;
        *copy = *this;
        return copy;
    }

    virtual const char* nodeType() const
    {
        return GeoDataTypes::GeoDataMultiGeometryType;
    }

    virtual EnumGeometryId geometryId() const
    {
        return GeoDataMultiGeometryId;
    }
    QVector<GeoDataGeometry*>  m_vector;
};

} // namespace Marble

#endif
