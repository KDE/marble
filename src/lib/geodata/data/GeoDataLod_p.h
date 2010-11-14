//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Torsten Rahn <rahn@kde.org>
//

#ifndef MARBLE_GEODATALODPRIVATE_H
#define MARBLE_GEODATALODPRIVATE_H

#include "GeoDataLod.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataLodPrivate
{
  public:
    GeoDataLodPrivate()
         : m_minLodPixels( 0 ),
           m_maxLodPixels( -1 ),
           m_minFadeExtent( 0 ),
           m_maxFadeExtent( 0 )
    {
    }

    QString nodeType() const
    {
        return GeoDataTypes::GeoDataLodType;
    }

    qreal m_minLodPixels;
    qreal m_maxLodPixels;
    qreal m_minFadeExtent;
    qreal m_maxFadeExtent;
};

} // namespace Marble

#endif
