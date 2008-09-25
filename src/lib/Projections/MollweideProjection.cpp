//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>
//

#include "MollweideProjection.h"

#include "MollweideProjectionHelper.h"

#include <math.h>

using namespace Marble;

static MollweideProjectionHelper theHelper;

MollweideProjection::MollweideProjection()
{
    m_maxLat  = 90.0 * DEG2RAD;
    m_minLat  = -90.0 * DEG2RAD;
    m_traversableMaxLat = false;
    m_repeatX = false;
}

MollweideProjection::~MollweideProjection()
{
}


AbstractProjectionHelper* MollweideProjection::helper()
{
    return &theHelper;
}

bool MollweideProjection::screenCoordinates( const qreal lon, const qreal lat,
                                       const ViewportParams *params,
                                       int& x, int& y,
                                       CoordinateType coordType )
{

}

bool MollweideProjection::geoCoordinates( const int x, const int y,
                                    const ViewportParams *params,
                                    qreal& lon, qreal& lat,
                                    GeoDataCoordinates::Unit unit )
{

}


