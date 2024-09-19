// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef GEODATAORIENTATION_H
#define GEODATAORIENTATION_H

#include "GeoDataObject.h"
#include "MarbleGlobal.h"

namespace Marble
{

class GeoDataOrientationPrivate;

/**
 */
class GEODATA_EXPORT GeoDataOrientation : public GeoDataObject
{
public:
    GeoDataOrientation();

    GeoDataOrientation(const GeoDataOrientation &other);

    GeoDataOrientation &operator=(const GeoDataOrientation &other);

    bool operator==(const GeoDataOrientation &other) const;
    bool operator!=(const GeoDataOrientation &other) const;

    ~GeoDataOrientation() override;

    /** Provides type information for downcasting a GeoNode */
    const char *nodeType() const override;

    /** Returns by how much degrees the camera has been rotated about the normal.
     *  Default value= 0. Range: 0-360 degrees
     */
    double heading() const;
    void setHeading(double heading);

    /** Returns the rotation of the camera in degrees, around the X axis.
     *  Value ranges from 0 (indicating that the view is aimed straight down
     *  toward the earth), through 90 (indicating that the view is aimed toward
     *  the horizon) to values greater than 90 (indicating that the view is pointed
     *  up into the sky. Values are clamped at +180 degrees.
     */
    double tilt() const;
    void setTilt(double tilt);

    /** Returns the rotation of the camera in degrees around the Z axis.
     *  Range: −180 to +180 degrees.
     */
    double roll() const;
    void setRoll(double roll);

protected:
    GeoDataOrientationPrivate *const d;
};

}

#endif // GEODATAORIENTATION_H
