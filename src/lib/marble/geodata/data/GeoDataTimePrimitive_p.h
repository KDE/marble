//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef GEODATATIMEPRIMITIVEPRIVATE_H
#define GEODATATIMEPRIMITIVEPRIVATE_H

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataTimePrimitivePrivate
{
  public:
    const char* nodeType() const
    {
        return GeoDataTypes::GeoDataTimePrimitiveType;
    }

};

} // namespace Marble

#endif //GEODATATIMEPRIMITIVEPRIVATE_H

