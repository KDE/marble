// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef MARBLE_GEODATAPHOTOOVERLAY_H
#define MARBLE_GEODATAPHOTOOVERLAY_H

#include "GeoDataOverlay.h"
#include "MarbleGlobal.h"
#include "geodata_export.h"

namespace Marble
{

class GeoDataPhotoOverlayPrivate;
class GeoDataPoint;
class GeoDataImagePyramid;
class GeoDataViewVolume;

/**
 */
class GEODATA_EXPORT GeoDataPhotoOverlay : public GeoDataOverlay
{
public:
    GeoDataPhotoOverlay();

    GeoDataPhotoOverlay(const GeoDataPhotoOverlay &other);

    ~GeoDataPhotoOverlay() override;

    GeoDataPhotoOverlay &operator=(const GeoDataPhotoOverlay &other);
    bool operator==(const GeoDataPhotoOverlay &other) const;
    bool operator!=(const GeoDataPhotoOverlay &other) const;

    GeoDataFeature *clone() const override;

    /** Provides type information for downcasting a GeoNode */
    const char *nodeType() const override;

    enum Shape {
        Rectangle,
        Cylinder,
        Sphere
    };

    qreal rotation() const;
    void setRotation(const qreal rotation);

    GeoDataViewVolume &viewVolume();
    const GeoDataViewVolume &viewVolume() const;
    void setViewVolume(const GeoDataViewVolume &viewVolume);

    GeoDataImagePyramid &imagePyramid();
    const GeoDataImagePyramid &imagePyramid() const;
    void setImagePyramid(const GeoDataImagePyramid &imagePyramid);

    GeoDataPoint &point();
    const GeoDataPoint &point() const;
    void setPoint(const GeoDataPoint &point);

    Shape shape() const;
    void setShape(Shape shape);

private:
    Q_DECLARE_PRIVATE(GeoDataPhotoOverlay)
};

}

#endif
