//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
//


#include "PlotPoint.h"
namespace Marble {

PlotPoint::PlotPoint( double x, double y, const GeoDataCoordinates& coordinates )
    : KPlotPoint( x, y ), m_coordinates( coordinates )
{

}

}
