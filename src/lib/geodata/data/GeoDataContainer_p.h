//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATACONTAINERPRIVATE_H
#define MARBLE_GEODATACONTAINERPRIVATE_H

#include "GeoDataFeature_p.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataContainerPrivate : public GeoDataFeaturePrivate
{
  public:
    GeoDataContainerPrivate()
    {
    }
    
    ~GeoDataContainerPrivate()
    {
        qDeleteAll( m_vector );
    }

    void operator=( const GeoDataContainerPrivate &other)
    {
        GeoDataFeaturePrivate::operator=( other );
        qDeleteAll( m_vector );
        foreach( GeoDataFeature *feature, other.m_vector )
        {
            m_vector.append( new GeoDataFeature( *feature ) );
        }
    }

    virtual GeoDataFeaturePrivate* copy()
    { 
        GeoDataContainerPrivate* copy = new GeoDataContainerPrivate;
        *copy = *this;
        return copy;
    }

    virtual const char* nodeType() const
    {
        return GeoDataTypes::GeoDataContainerType;
    }

    virtual EnumFeatureId featureId() const
    {
        return GeoDataFolderId;
    }

    QVector<GeoDataFeature*> m_vector;
};

} // namespace Marble

#endif
