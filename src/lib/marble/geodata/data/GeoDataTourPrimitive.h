// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef GEODATATOURPRIMITIVE_H
#define GEODATATOURPRIMITIVE_H

#include "GeoDataObject.h"
#include "geodata_export.h"

namespace Marble
{

/**
 */
class GEODATA_EXPORT GeoDataTourPrimitive : public GeoDataObject
{
public:
    bool operator==(const GeoDataTourPrimitive &other) const;
    inline bool operator!=(const GeoDataTourPrimitive &other) const
    {
        return !(*this == other);
    }
};

} // namespace Marble

#endif // GEODATATOURPRIMITIVE_H
