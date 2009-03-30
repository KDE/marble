//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef GEODATACONTAINERPRIVATE_H
#define GEODATACONTAINERPRIVATE_H

#include "GeoDataFeature_p.h"

namespace Marble
{

class GeoDataContainerPrivate : public Marble::GeoDataFeaturePrivate
{
  public:
    GeoDataContainerPrivate()
    {
    }
    
    virtual void* copy() 
    { 
        GeoDataContainerPrivate* copy = new GeoDataContainerPrivate;
        *copy = *this;
        return copy;
    }

    virtual EnumFeatureId featureId() const
    {
        return GeoDataFolderId;
    }

    QVector<GeoDataFeature> m_vector;
};

} // namespace Marble

#endif //GEODATACONTAINERPRIVATE_H
//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef GEODATACONTAINERPRIVATE_H
#define GEODATACONTAINERPRIVATE_H

#include "GeoDataFeature_p.h"

namespace Marble
{

class GeoDataContainerPrivate : public Marble::GeoDataFeaturePrivate
{
  public:
    GeoDataContainerPrivate()
    {
    }
    
    virtual void* copy() 
    { 
        GeoDataContainerPrivate* copy = new GeoDataContainerPrivate;
        *copy = *this;
        return copy;
    };

    QVector<GeoDataFeature> m_vector;
};

} // namespace Marble

#endif //GEODATACONTAINERPRIVATE_H
