//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2012	   Cezar Mocan <mocancezar@gmail.com>
//

#ifndef MARBLE_CYLINDRICALPROJECTIONPRIVATE_H
#define MARBLE_CYLINDRICALPROJECTIONPRIVATE_H

#include "AbstractProjection_p.h"


namespace Marble
{

class CylindricalProjection;

class CylindricalProjectionPrivate : public AbstractProjectionPrivate
{
  public:
    explicit CylindricalProjectionPrivate( CylindricalProjection * parent );

    bool lineStringToPolygon( const GeoDataLineString &lineString,
                              const ViewportParams *viewport,
                              QVector<QPolygonF*> &polygons ) const;

    void repeatPolygons( const ViewportParams *viewport,
                         QVector<QPolygonF *> &polygons ) const;

    Q_DECLARE_PUBLIC( CylindricalProjection )
};

} // namespace Marble

#endif
