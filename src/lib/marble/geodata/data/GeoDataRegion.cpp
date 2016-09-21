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


const char* GeoDataRegion::nodeType() const
{
    return d->nodeType();
}

bool GeoDataRegion::operator==(const GeoDataRegion& other) const
{
    return equals(other)
           && this->latLonAltBox() == other.latLonAltBox()
           && this->lod() == other.lod();
}

bool GeoDataRegion::operator!=(const GeoDataRegion& other) const
{
    return !this->operator==(other);
}

const GeoDataLatLonAltBox& GeoDataRegion::latLonAltBox() const
{
    // FIXME: This isn't exactly what a 'const' function should do, is it?

    // If the latLonAltBox hasn't been set try to determine it automatically
    if ( !d->m_latLonAltBox ) {
        // If there is a parent try to 
        if ( d->m_parent ) {

            if ( d->m_parent->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {

                GeoDataPlacemark * placemark = dynamic_cast<GeoDataPlacemark*>( d->m_parent );
                const GeoDataGeometry * geometry = placemark->geometry();
                if ( geometry ) {
                    d->m_latLonAltBox = new GeoDataLatLonAltBox( placemark->geometry()->latLonAltBox() );
                }
                else {
                    d->m_latLonAltBox = new GeoDataLatLonAltBox();
                }
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
    delete d->m_latLonAltBox;
    d->m_latLonAltBox = new GeoDataLatLonAltBox( latLonAltBox );
}


GeoDataLod& GeoDataRegion::lod() const
{
    // If the lod hasn't been set then return a shared one
    if ( !d->m_lod ) {
        // FIXME: reference a shared object instead
        d->m_lod = new GeoDataLod();
    }

    return *(d->m_lod);
}


void GeoDataRegion::setLod( const GeoDataLod & lod )
{
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

    GeoDataRegion temp( other );
    swap( temp );
    return *this;
}

void GeoDataRegion::swap( GeoDataRegion & other )
{
    std::swap( d, other.d );
}

}

