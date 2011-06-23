//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
//


#ifndef MARBLE_PLOTPOINT_H
#define MARBLE_PLOTPOINT_H

#include <KPlotPoint>
#include <GeoDataCoordinates.h>

namespace Marble {

class PlotPoint : public KPlotPoint
{
public:
    PlotPoint( double x, double y, const GeoDataCoordinates& coordinates );
    GeoDataCoordinates coordinates() const { return m_coordinates; };
private:
    GeoDataCoordinates m_coordinates;
};

}

#endif // MARBLE_PLOTPOINT_H
