//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#include "ViewportParams.h"


ViewportParams::ViewportParams( )
{
    // Default projection
    m_projection = Spherical;

    // Default view
    m_planetAxis = Quaternion( 1.0, 0.0, 0.0, 0.0 );
    m_radius     = 2000;

    m_width      = 100;
    m_height     = 100;
}

ViewportParams::~ViewportParams()
{
}


void ViewportParams::centerCoordinates( double &centerLon, double &centerLat )
{
    // Calculate translation of center point
    centerLat = - m_planetAxis.pitch();
    if ( centerLat > M_PI )
        centerLat -= 2 * M_PI;

    centerLon = + m_planetAxis.yaw();
    if ( centerLon > M_PI )
        centerLon -= 2 * M_PI;

    // qDebug() << "centerLon" << centerLon * RAD2DEG;
    // qDebug() << "centerLat" << centerLat * RAD2DEG;
}
