//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef GEODATADOCUMENTPRIVATE_H
#define GEODATADOCUMENTPRIVATE_H

#include "GeoDataContainer_p.h"

namespace Marble
{

class GeoDataDocumentPrivate : public Marble::GeoDataContainerPrivate
{
  public:
    GeoDataDocumentPrivate()
    {
    }
    
    virtual void* copy() 
    { 
        GeoDataDocumentPrivate* copy = new GeoDataDocumentPrivate;
        *copy = *this;
        return copy;
    };

    QHash<QString, GeoDataStyle*> m_styleHash;
    QHash<QString, GeoDataStyleMap*> m_styleMapHash;
    QString m_filename;
};

} // namespace Marble

#endif //GEODATACONTAINERPRIVATE_H
