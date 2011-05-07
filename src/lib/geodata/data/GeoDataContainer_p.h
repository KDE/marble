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
        qDeleteAll(m_vector);
    }

    virtual void* copy() 
    { 
        return new GeoDataContainerPrivate( *this );
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
