// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef MARBLE_GEODATAPHOTOOVERLAY_P_H
#define MARBLE_GEODATAPHOTOOVERLAY_P_H

#include "GeoDataOverlay_p.h"
#include "GeoDataTypes.h"
#include "GeoDataPoint.h"
#include "GeoDataImagePyramid.h"
#include "GeoDataViewVolume.h"

namespace Marble {

class GeoDataPhotoOverlayPrivate : public GeoDataOverlayPrivate
{
public:
    GeoDataPhotoOverlayPrivate();
    GeoDataPhotoOverlayPrivate(const GeoDataPhotoOverlayPrivate& other);

    qreal m_rotation;
    GeoDataViewVolume m_viewVolume;
    GeoDataImagePyramid m_imagePyramid;
    GeoDataPoint m_point;
    GeoDataPhotoOverlay::Shape m_shape;
};

GeoDataPhotoOverlayPrivate::GeoDataPhotoOverlayPrivate() :
    m_rotation( 0.0 ),
    m_viewVolume(),
    m_imagePyramid(),
    m_point(),
    m_shape( GeoDataPhotoOverlay::Rectangle )
{
    // nothing to do
}

GeoDataPhotoOverlayPrivate::GeoDataPhotoOverlayPrivate(const GeoDataPhotoOverlayPrivate& other)
  : GeoDataOverlayPrivate(other),
    m_rotation(other.m_rotation),
    m_viewVolume(other.m_viewVolume),
    m_imagePyramid(other.m_imagePyramid),
    m_point(other.m_point),
    m_shape(other.m_shape)
{
}

}

#endif
