//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Torsten Rahn <rahn@kde.org>
//

#ifndef MARBLE_GEODATAREGIONPRIVATE_H
#define MARBLE_GEODATAREGIONPRIVATE_H

#include "GeoDataRegion.h"

#include "GeoDataTypes.h"

//Qt
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>

namespace Marble
{

class GeoDataRegionPrivate
{
  public:
    GeoDataRegionPrivate()
         : m_parent( 0 ),
           m_latLonAltBox( 0 ),
           m_lod( 0 )
    {
    }

    GeoDataRegionPrivate( const GeoDataRegionPrivate& other )
         : m_parent( other.m_parent )
    {
        if ( other.m_latLonAltBox ) {
            m_latLonAltBox = new GeoDataLatLonAltBox( *other.m_latLonAltBox );
        }
        else {
            m_latLonAltBox = 0;
        }

        if ( other.m_lod ) {
            m_lod = new GeoDataLod( *other.m_lod );
        }
        else {
            m_lod = 0;
        }
    }


    explicit GeoDataRegionPrivate( GeoDataFeature * feature )
         : m_parent( feature ),
           m_latLonAltBox( 0 ),
           m_lod( 0 )
    {
    }

    ~GeoDataRegionPrivate()
    {
        QMutexLocker locker( &m_mutex );
        delete m_latLonAltBox;
        delete m_lod;
    }


    QString nodeType() const
    {
        return GeoDataTypes::GeoDataRegionType;
    }

    GeoDataFeature * m_parent;
    GeoDataLatLonAltBox * m_latLonAltBox;
    GeoDataLod * m_lod;

    QMutex m_mutex;

  private:
    GeoDataRegionPrivate &operator=( const GeoDataRegionPrivate& )
    {
        return *this;
    }
};

} // namespace Marble

#endif
