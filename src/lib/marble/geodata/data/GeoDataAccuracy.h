//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_GEODATAACCURACY_H
#define MARBLE_GEODATAACCURACY_H

#include "geodata_export.h"

#include <QtGlobal>

namespace Marble
{

/**
 */
class GEODATA_EXPORT GeoDataAccuracy
{
 public:
    enum Level {
        none = 0,
        Country,
        Region,
        Locality,
        PostalCode,
        Street,
        Detailed
    };

    explicit GeoDataAccuracy( Level level = none, qreal horizontal = 0.0, qreal vertical = 0.0 );

    bool operator==( const GeoDataAccuracy &other ) const;

    bool operator!=( const GeoDataAccuracy &other ) const;

    /**
     * @brief Approximate descriptive accuracy.
     */
    Level level;

    /**
     * @brief Horizontal accuracy in meters.
     */
    qreal horizontal;

    /**
     * @brief Vertical accuracy in meters.
     */
    qreal vertical;
};

}

#endif
