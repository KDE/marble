//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef GEODATATOUR_H
#define GEODATATOUR_H

#include "GeoDataFeature.h"
#include "GeoDataPlaylist.h"
#include "geodata_export.h"

namespace Marble
{

class GeoDataTourPrivate;
class GeoDataPlaylist;
class GeoDataSoundCue;

class GEODATA_EXPORT GeoDataTour : public GeoDataFeature
{
public:
    GeoDataTour();
    GeoDataTour(const GeoDataTour &other);
    GeoDataTour& operator=(const GeoDataTour &other);
    bool operator==(const GeoDataTour &other) const;
    bool operator!=(const GeoDataTour &other) const;
    virtual ~GeoDataTour();

    GeoDataPlaylist* playlist();
    const GeoDataPlaylist* playlist() const;
    void setPlaylist(GeoDataPlaylist* playlist);

    virtual const char* nodeType() const;

    static const GeoDataTour null;

private:
    GeoDataTourPrivate * const d;

};

} // namespace Marble

#endif // GEODATATOUR_H
