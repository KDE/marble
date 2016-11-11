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

    GeoDataContainerPrivate(const GeoDataContainerPrivate& other)
      : GeoDataFeaturePrivate(other)
    {
        m_vector.reserve(other.m_vector.size());
        foreach (GeoDataFeature *feature, other.m_vector) {
            m_vector.append(feature->clone());
        }
    }

    ~GeoDataContainerPrivate()
    {
        qDeleteAll( m_vector );
    }

    GeoDataContainerPrivate& operator=( const GeoDataContainerPrivate &other)
    {
        GeoDataFeaturePrivate::operator=( other );
        qDeleteAll( m_vector );
        m_vector.clear();
        m_vector.reserve(other.m_vector.size());
        foreach( GeoDataFeature *feature, other.m_vector )
        {
            m_vector.append(feature->clone());
        }
        return *this;
    }

    virtual EnumFeatureId featureId() const
    {
        return GeoDataFolderId;
    }

    void setParent(GeoDataObject *parent)
    {
        foreach (GeoDataFeature *feature, m_vector) {
            feature->setParent(parent);
        }
    }

    QVector<GeoDataFeature*> m_vector;
};

} // namespace Marble

#endif
