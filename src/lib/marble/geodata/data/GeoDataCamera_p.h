//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//


#ifndef MARBLE_GEODATACAMERA_P_H
#define MARBLE_GEODATACAMERA_P_H

#include "GeoDataCoordinates.h"

#include <QAtomicInt>

namespace Marble
{

class GeoDataCameraPrivate
{
 public :
    GeoDataCameraPrivate()
        : m_coordinates(),
          m_roll( 0.0 ),
          m_heading( 0.0 ),
          m_tilt( 0.0 ),
          ref( 1 )
    {
    }

    GeoDataCoordinates m_coordinates;
    qreal m_roll;
    qreal m_heading;
    qreal m_tilt;

    QAtomicInt ref;
};

} // namespace Marble

#endif
