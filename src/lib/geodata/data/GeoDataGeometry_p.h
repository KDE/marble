//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef GEODATAGEOMETRYPRIVATE_H
#define GEODATAGEOMETRYPRIVATE_H

#include <QtCore/QString>
#if QT_VERSION < 0x040400
# include <qatomic.h>
#else
# include <QtCore/QAtomicInt>
#endif

#include "GeoDataGeometry.h"

namespace Marble
{
#if QT_VERSION < 0x040400
    typedef QAtomic QAtomicInt;
#endif

class GeoDataGeometryPrivate
{
  public:
    GeoDataGeometryPrivate()
        : m_extrude( false ),
          m_altitudeMode( ClampToGround ),
          ref( 0 )
    {
    }

    virtual ~GeoDataGeometryPrivate()
    {
    }

    virtual void* copy() 
    { 
        GeoDataGeometryPrivate* copy = new GeoDataGeometryPrivate;
        *copy = *this;
        return copy;
    }

    virtual EnumGeometryId geometryId() const
    {
        return InvalidGeometryId;
    }

    bool         m_extrude;
    AltitudeMode m_altitudeMode;
    
    QAtomicInt  ref;
};

} // namespace Marble

#endif //GeoDataGeometryPRIVATE_H
