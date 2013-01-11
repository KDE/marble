//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees  <nafees.technocool@gmail.com>
//

#include "GeoDataImagePyramid.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataImagePyramidPrivate
{
public:
    GeoDataImagePyramidPrivate();

    int m_tileSize;
    int m_maxWidth;
    int m_maxHeight;
    GeoDataImagePyramid::GridOrigin m_gridOrigin;
};

GeoDataImagePyramidPrivate::GeoDataImagePyramidPrivate() :
    m_tileSize( 256 ),
    m_maxWidth(),
    m_maxHeight(),
    m_gridOrigin()
{
    // nothing to do
}

GeoDataImagePyramid::GeoDataImagePyramid() : d( new GeoDataImagePyramidPrivate )
{
    // nothing to do
}

GeoDataImagePyramid::GeoDataImagePyramid( const Marble::GeoDataImagePyramid &other ) :
    GeoDataObject(), d( new GeoDataImagePyramidPrivate( *other.d ) )
{
    // nothing to do
}

GeoDataImagePyramid &GeoDataImagePyramid::operator=( const GeoDataImagePyramid &other )
{
    GeoDataObject::operator=( other );
    *d = *other.d;
    return *this;
}

GeoDataImagePyramid::~GeoDataImagePyramid()
{
    delete d;
}

const char *GeoDataImagePyramid::nodeType() const
{
    return GeoDataTypes::GeoDataImagePyramidType;
}

int GeoDataImagePyramid::tileSize() const
{
    return d->m_tileSize;
}

void GeoDataImagePyramid::setTileSize( const int &tileSize )
{
    d->m_tileSize = tileSize;
}

int GeoDataImagePyramid::maxWidth() const
{
    return d->m_maxWidth;
}

void GeoDataImagePyramid::setMaxWidth( const int &maxWidth )
{
    d->m_maxWidth = maxWidth;
}

int GeoDataImagePyramid::maxHeight() const
{
    return d->m_maxHeight;
}

void GeoDataImagePyramid::setMaxHeight( const int &maxHeight )
{
    d->m_maxHeight = maxHeight;
}

GeoDataImagePyramid::GridOrigin GeoDataImagePyramid::gridOrigin() const
{
    return d->m_gridOrigin;
}

void GeoDataImagePyramid::setGridOrigin( const GridOrigin &gridOrigin )
{
    d->m_gridOrigin = gridOrigin;
}

}
