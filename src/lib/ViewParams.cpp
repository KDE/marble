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

#include <QImage>


ViewParams::ViewParams( )
{
    m_projection = Spherical;
    m_oldProjection = Spherical;

    m_planetAxis = Quaternion( 1.0, 0.0, 0.0, 0.0 );
    m_radius     = 2000;

    // Show / don't show parameters
    m_showGrid           = true;
    m_showPlaceMarks     = true;
    m_showElevationModel = false;

    m_showRelief         = true;

    m_showIceLayer       = true;
    m_showBorders        = true;
    m_showRivers         = true;
    m_showLakes          = true;

    m_showCities         = true;
    m_showTerrain        = true;
    m_showOtherPlaces    = true;
    
    m_showGps            = false;

    // Just to have something.
    m_canvasImage = new QImage( 10, 10, QImage::Format_ARGB32_Premultiplied );
    m_coastImage  = new QImage( 10, 10, QImage::Format_ARGB32_Premultiplied );
}

ViewParams::~ViewParams()
{
    delete m_canvasImage;
    delete m_coastImage;
    m_canvasImage = 0;
    m_coastImage = 0;
}
