// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2007-2012 Torsten Rahn <rahn@kde.org>
// SPDX-FileCopyrightText: 2012 Cezar Mocan <mocancezar@gmail.com>
//

#ifndef MARBLE_CYLINDRICALPROJECTION_H
#define MARBLE_CYLINDRICALPROJECTION_H

/** @file
 * This file contains the headers for CylindricalProjection.
 *
 */

#include "AbstractProjection.h"

namespace Marble
{

class CylindricalProjectionPrivate;
class AbstractProjectionPrivate;

/**
 * @short A base class for the Equirectangular and Mercator projections in Marble
 */

class CylindricalProjection : public AbstractProjection
{
    // Not a QObject so far because we don't need to send signals.
public:
    CylindricalProjection();

    ~CylindricalProjection() override;

    bool repeatableX() const override
    {
        return true;
    };

    bool traversablePoles() const override
    {
        return false;
    }
    bool traversableDateLine() const override
    {
        return false;
    }

    SurfaceType surfaceType() const override
    {
        return Cylindrical;
    }

    bool screenCoordinates(const GeoDataLineString &lineString, const ViewportParams *viewport, QList<QPolygonF *> &polygons) const override;

    using AbstractProjection::screenCoordinates;

    QPainterPath mapShape(const ViewportParams *viewport) const override;

protected:
    explicit CylindricalProjection(CylindricalProjectionPrivate *dd);

private:
    Q_DECLARE_PRIVATE(CylindricalProjection)
    Q_DISABLE_COPY(CylindricalProjection)
};

}

#endif
