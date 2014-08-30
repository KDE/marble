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

GeoDataTour::~GeoDataTour()
{
    // nothing to do;
}

GeoDataTourPrivate *GeoDataTour::p()
{
    return static_cast<GeoDataTourPrivate*>(d);
}

const GeoDataTourPrivate *GeoDataTour::p() const
{
    return static_cast<GeoDataTourPrivate*>(d);
}

bool GeoDataTour::operator==(const GeoDataTour& other) const
{
    return equals( other ) &&
           *p()->m_playlist == *other.p()->m_playlist;
}

bool GeoDataTour::operator!=(const GeoDataTour& other) const
{
    return !this->operator==(other);
}

GeoDataPlaylist* GeoDataTour::playlist()
{
    detach();
    return p()->m_playlist;
}

const GeoDataPlaylist* GeoDataTour::playlist() const
{
    return p()->m_playlist;
}

void GeoDataTour::setPlaylist(GeoDataPlaylist *playlist)
{
    detach();
    p()->m_playlist = playlist;
    p()->m_playlist->setParent( this );
}

const char *GeoDataTour::nodeType() const
{
    return GeoDataTypes::GeoDataTourType;
}

} // namespace Marble
