//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef GEODATATIMESPANPRIVATE_H
#define GEODATATIMESPANPRIVATE_H

#include "GeoDataTypes.h"
#include "GeoDataTimeStamp.h"

namespace Marble
{

class GeoDataTimeSpanPrivate
{
  public:
    const char* nodeType() const
    {
        return GeoDataTypes::GeoDataTimeSpanType;
    }

    GeoDataTimeStamp m_begin;
    GeoDataTimeStamp m_end;
};

} // namespace Marble

#endif //GEODATATIMESPANPRIVATE_H

