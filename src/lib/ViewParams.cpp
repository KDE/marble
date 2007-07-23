//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#include "ViewParams.h"

ViewParams::ViewParams( )
{
    m_planetAxis = Quaternion( 1.0, 0.0, 0.0, 0.0 );
    m_radius     = 2000;

    // Show / don't show parameters
    m_showGrid           = true;
    m_showPlaceMarks     = true;
    m_showElevationModel = false;
}
