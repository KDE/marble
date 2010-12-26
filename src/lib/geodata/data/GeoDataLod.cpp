//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Torsten Rahn   <rahn@kde.org>
//


#include "GeoDataLod.h"
#include "GeoDataLod_p.h"


namespace Marble
{
GeoDataLod::GeoDataLod()
    : GeoDataObject(),
      d( new GeoDataLodPrivate )
{
}

GeoDataLod::GeoDataLod( const GeoDataLod& other )
    : GeoDataObject( other ),
      d( new GeoDataLodPrivate( *other.d ) )
{
}

GeoDataLod::~GeoDataLod()
{
    delete d;
}


const char* GeoDataLod::nodeType() const
{
    return d->nodeType();
}


qreal GeoDataLod::minLodPixels() const
{
    return d->m_minLodPixels;
}


void GeoDataLod::setMinLodPixels( qreal pixels )
{
    d->m_minLodPixels = pixels;
}


qreal GeoDataLod::maxLodPixels() const
{
    return d->m_maxLodPixels;
}


void GeoDataLod::setMaxLodPixels( qreal pixels )
{
    d->m_maxLodPixels = pixels;
}


qreal GeoDataLod::minFadeExtent() const
{
    return d->m_minFadeExtent;
}


void GeoDataLod::setMinFadeExtent( qreal pixels )
{
    d->m_minFadeExtent = pixels;
}


qreal GeoDataLod::maxFadeExtent() const
{
    return d->m_maxFadeExtent;
}


void GeoDataLod::setMaxFadeExtent( qreal pixels )
{
    d->m_maxFadeExtent = pixels;
}


void GeoDataLod::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << d->m_minLodPixels << d->m_maxLodPixels;
    stream << d->m_minFadeExtent << d->m_maxFadeExtent;
}

void GeoDataLod::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );

    stream >> d->m_minLodPixels >> d->m_maxLodPixels;
    stream >> d->m_minFadeExtent >> d->m_maxFadeExtent;
}

GeoDataLod &GeoDataLod::operator=( const GeoDataLod& other )
{
    *d = *other.d;
    return *this;
}

}

