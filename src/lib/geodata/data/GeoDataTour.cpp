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

namespace Marble
{

class GeoDataTourPrivate
{
public:
    GeoDataTourPrivate() :
        m_id(""),
        m_playlist(0)
    {}
    QString m_id;
    GeoDataPlaylist *m_playlist;
};

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

GeoDataTour::~GeoDataTour()
{
    delete d;
}

QString GeoDataTour::id() const
{
    return d->m_id;
}

void GeoDataTour::setId(const QString &value)
{
    d->m_id = value;
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
}

} // namespace Marble
