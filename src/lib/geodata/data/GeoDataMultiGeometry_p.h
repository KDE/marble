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

    void operator=( const GeoDataMultiGeometryPrivate &other)
    {
        GeoDataGeometryPrivate::operator=( other );
        qDeleteAll( m_vector );
        foreach( GeoDataGeometry *geometry, other.m_vector ) {
            m_vector.append( new GeoDataGeometry( *geometry ) );
        }
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
