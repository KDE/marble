//
// This file is part of the Marble Desktop Globe.
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
        QVector<GeoDataFeature*>::Iterator i = m_vector.begin();
        QVector<GeoDataFeature*>::Iterator end = m_vector.end();
        for( ; i != end; ++i )
        {
            delete *i;
        }
    }

    virtual void* copy() 
    { 
        GeoDataContainerPrivate* copy = new GeoDataContainerPrivate;
        *copy = *this;
        return copy;
    }

    virtual QString nodeType() const
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
