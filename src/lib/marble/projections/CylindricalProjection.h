//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007       Inge Wallin  <ingwa@kde.org>
// Copyright 2007-2012  Torsten Rahn  <rahn@kde.org>
// Copyright 2012		Cezar Mocan	<mocancezar@gmail.com>
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

    bool repeatableX() const override { return true; };

    bool traversablePoles()  const override { return false; }
    bool traversableDateLine()  const override { return false; }

    SurfaceType surfaceType() const override { return Cylindrical; }

    bool screenCoordinates( const GeoDataLineString &lineString,
                            const ViewportParams *viewport,
                            QVector<QPolygonF*> &polygons ) const override;

    using AbstractProjection::screenCoordinates;

    QPainterPath mapShape( const ViewportParams *viewport ) const override;

 protected: 
    explicit CylindricalProjection( CylindricalProjectionPrivate* dd );

 private:
    Q_DECLARE_PRIVATE( CylindricalProjection )
    Q_DISABLE_COPY( CylindricalProjection )
};

}

#endif

