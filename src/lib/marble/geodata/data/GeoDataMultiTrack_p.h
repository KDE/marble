//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLE_GEODATAMULTITRACKPRIVATE_H
#define MARBLE_GEODATAMULTITRACKPRIVATE_H

#include "GeoDataGeometry_p.h"

#include "GeoDataTypes.h"
#include "GeoDataTrack.h"

namespace Marble
{

class GeoDataMultiTrackPrivate : public GeoDataGeometryPrivate
{
  public:
    GeoDataMultiTrackPrivate()
    {
    }

    ~GeoDataMultiTrackPrivate()
    {
        qDeleteAll(m_vector);
    }

    void operator=( const GeoDataMultiTrackPrivate &other)
    {
        GeoDataGeometryPrivate::operator=( other );
        qDeleteAll( m_vector );
        foreach( GeoDataTrack *track, other.m_vector ) {
            m_vector.append( new GeoDataTrack( *track ) );
        }
    }

    virtual GeoDataGeometryPrivate* copy()
    { 
         GeoDataMultiTrackPrivate* copy = new GeoDataMultiTrackPrivate;
        *copy = *this;
        return copy;
    }

    virtual const char* nodeType() const
    {
        return GeoDataTypes::GeoDataMultiTrackType;
    }

    virtual EnumGeometryId geometryId() const
    {
        return GeoDataMultiTrackId;
    }
    QVector<GeoDataTrack*>  m_vector;
};

} // namespace Marble

#endif
