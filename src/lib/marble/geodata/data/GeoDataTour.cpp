//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "GeoDataTour.h"
#include "GeoDataPlaylist.h"
#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataTourPrivate
{
public:
    GeoDataTourPrivate() :
        m_playlist(0)
    {}
    GeoDataPlaylist *m_playlist;
};

const GeoDataTour GeoDataTour::null;

GeoDataTour::GeoDataTour() :
    GeoDataFeature(),
    d(new GeoDataTourPrivate)
{
}

GeoDataTour::GeoDataTour(const GeoDataTour &other) :
    GeoDataFeature(),
    d(new GeoDataTourPrivate(*other.d))
{
}

GeoDataTour& GeoDataTour::operator=(const GeoDataTour &other)
{
    GeoDataFeature::operator=(other);
    *d = *other.d;
    return *this;
}

bool GeoDataTour::operator==(const GeoDataTour& other) const
{
    return equals( other ) &&
           *d->m_playlist == *other.d->m_playlist;
}

bool GeoDataTour::operator!=(const GeoDataTour& other) const
{
    return !this->operator==(other);
}

GeoDataTour::~GeoDataTour()
{
    delete d;
}

GeoDataPlaylist* GeoDataTour::playlist()
{
    return d->m_playlist;
}

const GeoDataPlaylist* GeoDataTour::playlist() const
{
    return d->m_playlist;
}

void GeoDataTour::setPlaylist(GeoDataPlaylist *playlist)
{
    d->m_playlist = playlist;
    d->m_playlist->setParent( this );
}

const char *GeoDataTour::nodeType() const
{
    return GeoDataTypes::GeoDataTourType;
}

} // namespace Marble
