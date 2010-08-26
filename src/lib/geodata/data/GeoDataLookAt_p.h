//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Gaurav Gupta <1989.gaurav@googlemail.com>
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_GEODATALOOKAT_P_H
#define MARBLE_GEODATALOOKAT_P_H

// Marble
#include "GeoDataCoordinates.h"
#include "GeoDataTypes.h"

// Qt
#include <QtCore/QAtomicInt>

namespace Marble
{

class GeoDataLookAtPrivate
{
 public :
    GeoDataLookAtPrivate()
        : m_coordinates(),
          m_range( 0.0 ),
          ref( 1 )
    {
    }
    
    GeoDataCoordinates m_coordinates;
    qreal m_range;

    QAtomicInt ref;
};

} // namespace Marble

#endif
