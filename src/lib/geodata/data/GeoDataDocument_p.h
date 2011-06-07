//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATADOCUMENTPRIVATE_H
#define MARBLE_GEODATADOCUMENTPRIVATE_H

#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataContainer_p.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataDocumentPrivate : public GeoDataContainerPrivate
{
  public:
    GeoDataDocumentPrivate()
    {
    }
    
    virtual GeoDataFeaturePrivate* copy()
    { 
        GeoDataDocumentPrivate* copy = new GeoDataDocumentPrivate;
        *copy = *this;
        return copy;
    }

    virtual const char* nodeType() const
    {
        return GeoDataTypes::GeoDataDocumentType;
    }

    virtual EnumFeatureId featureId() const
    {
        return GeoDataDocumentId;
    }

    QMap<QString, GeoDataStyle> m_styleHash;
    QMap<QString, GeoDataStyleMap> m_styleMapHash;
    QString m_filename;
};

} // namespace Marble

#endif
