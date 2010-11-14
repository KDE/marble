//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Torsten Rahn   <rahn@kde.org>
//


// Own
#include "GeoDataRegion.h"

// Private
#include "GeoDataRegion_p.h"

// GeoData
#include "GeoDataFeature.h"
#include "GeoDataPlacemark.h"
#include "GeoDataGeometry.h"

#include "GeoDataTypes.h"

// std
#include <algorithm>


namespace Marble
{
GeoDataRegion::GeoDataRegion()
    : GeoDataObject(),
      d( new GeoDataRegionPrivate )
{
}

GeoDataRegion::GeoDataRegion( const GeoDataRegion& other )
    : GeoDataObject( other ),
      d( new GeoDataRegionPrivate( *other.d ) )
{
}

GeoDataRegion::GeoDataRegion( GeoDataFeature * feature )
    : GeoDataObject(),
      d( new GeoDataRegionPrivate( feature ) )
{
}


GeoDataRegion::~GeoDataRegion()
{
    delete d;
}


QString GeoDataRegion::nodeType() const
{
    return d->nodeType();
}


GeoDataLatLonAltBox& GeoDataRegion::latLonAltBox() const
{
    QMutexLocker locker( &d->m_mutex );
    // FIXME: This isn't exactly what a 'const' function should do, is it?

    // If the latLonAltBox hasn't been set try to determine it automatically
    if ( !d->m_latLonAltBox ) {
        // If there is a parent try to 
        if ( d->m_parent ) {

            if ( d->m_parent->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {

                GeoDataPlacemark * placemark = dynamic_cast<GeoDataPlacemark*>( d->m_parent );
                GeoDataGeometry * geometry = placemark->geometry();
                if ( geometry ) {
                    // TODO: automatically calculate the geometry from the
                    //       GeoDataGeometry object.
                    // return geometry->latLonAltBox();
                }

                d->m_latLonAltBox = new GeoDataLatLonAltBox( placemark->coordinate() );
            }
            else {
                // If the parent is not a placemark then create a default LatLonAltBox
                // FIXME: reference a shared object instead
                d->m_latLonAltBox = new GeoDataLatLonAltBox();
            }
        }
        else {
            // If there is no parent then create a default LatLonAltBox
            // FIXME: reference a shared object instead
            d->m_latLonAltBox = new GeoDataLatLonAltBox();
        }
    }
    
    return *(d->m_latLonAltBox);
}


void GeoDataRegion::setLatLonAltBox( const GeoDataLatLonAltBox & latLonAltBox )
{
    QMutexLocker locker( &d->m_mutex );
    delete d->m_latLonAltBox;
    d->m_latLonAltBox = new GeoDataLatLonAltBox( latLonAltBox );
}


GeoDataLod& GeoDataRegion::lod() const
{
    QMutexLocker locker( &d->m_mutex );
    // If the lod hasn't been set then return a shared one
    if ( !d->m_lod ) {
        // FIXME: reference a shared object instead
        d->m_lod = new GeoDataLod();
    }

    return *(d->m_lod);
}


void GeoDataRegion::setLod( const GeoDataLod & lod )
{
    QMutexLocker locker( &d->m_mutex );
    delete d->m_lod;
    d->m_lod = new GeoDataLod( lod );
}


void GeoDataRegion::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    d->m_lod->pack( stream );
    d->m_latLonAltBox->pack( stream );
}


void GeoDataRegion::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );

    d->m_lod->unpack( stream );
    d->m_latLonAltBox->unpack( stream );
}

GeoDataRegion &GeoDataRegion::operator=( const GeoDataRegion& other )
{
    // Self assignment
    if ( this == &other ) return *this;

    other.d->m_mutex.lock();
    GeoDataRegion temp( other );
    other.d->m_mutex.unlock();
    swap( temp );
    return *this;
}

void GeoDataRegion::swap( GeoDataRegion & other )
{
    std::swap( d, other.d );
}

}

