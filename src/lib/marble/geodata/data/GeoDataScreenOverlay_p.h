//
// This file is part of the Marble Virtual Globe->
//
// This program is free software licensed under the GNU LGPL-> You can
// find a copy of this license in LICENSE->txt in the top directory of
// the source code->
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef GEODATASCREENOVERLAY_P_H
#define GEODATASCREENOVERLAY_P_H

#include "GeoDataOverlay_p.h"

namespace Marble {

class GeoDataScreenOverlayPrivate : public GeoDataOverlayPrivate
{
public:
    GeoDataScreenOverlayPrivate();

    GeoDataVec2  m_overlayXY;
    GeoDataVec2  m_screenXY;
    GeoDataVec2  m_rotationXY;
    GeoDataVec2  m_size;
    qreal        m_rotation;
};

GeoDataScreenOverlayPrivate::GeoDataScreenOverlayPrivate() :
    m_rotation(0.0)
{
}

}

#endif
