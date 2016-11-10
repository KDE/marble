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
#include "GeoDataTour_p.h"


namespace Marble
{

const GeoDataTour GeoDataTour::null;

GeoDataTour::GeoDataTour()
    : GeoDataFeature( new GeoDataTourPrivate )
{
    // nothing to do
}

GeoDataTour::GeoDataTour(const GeoDataTour& other)
    : GeoDataFeature(other, new GeoDataTourPrivate(*other.d_func()))
{
}

GeoDataTour::~GeoDataTour()
{
    // nothing to do;
}

GeoDataTour& GeoDataTour::operator=(const GeoDataTour& other)
{
    if (this != &other) {
        Q_D(GeoDataTour);
        *d = *other.d_func();
    }

    return *this;
}


bool GeoDataTour::operator==(const GeoDataTour& other) const
{
    Q_D(const GeoDataTour);
    return equals( other ) &&
           *d->m_playlist == *other.d_func()->m_playlist;
}

bool GeoDataTour::operator!=(const GeoDataTour& other) const
{
    return !this->operator==(other);
}

GeoDataFeature * GeoDataTour::clone() const
{
    return new GeoDataTour(*this);
}


GeoDataPlaylist* GeoDataTour::playlist()
{
    Q_D(GeoDataTour);
    return d->m_playlist;
}

const GeoDataPlaylist* GeoDataTour::playlist() const
{
    Q_D(const GeoDataTour);
    return d->m_playlist;
}

void GeoDataTour::setPlaylist(GeoDataPlaylist *playlist)
{
    Q_D(GeoDataTour);
    d->m_playlist = playlist;
    d->m_playlist->setParent(this);
}

const char *GeoDataTour::nodeType() const
{
    return GeoDataTypes::GeoDataTourType;
}

} // namespace Marble
