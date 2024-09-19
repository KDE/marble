// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>

#ifndef GEODATASCREENOVERLAY_P_H
#define GEODATASCREENOVERLAY_P_H

#include "GeoDataOverlay_p.h"

namespace Marble
{

class GeoDataScreenOverlayPrivate : public GeoDataOverlayPrivate
{
public:
    GeoDataScreenOverlayPrivate();

    GeoDataVec2 m_overlayXY;
    GeoDataVec2 m_screenXY;
    GeoDataVec2 m_rotationXY;
    GeoDataVec2 m_size;
    qreal m_rotation;
};

GeoDataScreenOverlayPrivate::GeoDataScreenOverlayPrivate()
    : m_rotation(0.0)
{
}

}

#endif
