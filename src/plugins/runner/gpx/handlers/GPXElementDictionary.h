/*
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef MARBLE_GPX_ELEMENTDICTIONARY_H
#define MARBLE_GPX_ELEMENTDICTIONARY_H


namespace Marble
{

// Lists all known GPX 1.1 tags (http://www.topografix.com/GPX/1/1/)
namespace gpx
{
    extern const char* gpxTag_nameSpace10;
    extern const char* gpxTag_nameSpace11;

    extern const char* gpxTag_gpx;
    extern const char* gpxTag_lat;
    extern const char* gpxTag_lon;
    extern const char* gpxTag_name;
    extern const char* gpxTag_trk;
    extern const char* gpxTag_trkpt;
    extern const char* gpxTag_trkseg;
    extern const char* gpxTag_wpt;
    extern const char* gpxTag_rte;
    extern const char* gpxTag_rtept;
    // TODO: add all remaining tags!
}

// Helper macros
#define GPX_DEFINE_TAG_HANDLER_10(Name) GEODATA_DEFINE_TAG_HANDLER(gpx, GPX, Name, gpxTag_nameSpace10)
#define GPX_DEFINE_TAG_HANDLER_11(Name) GEODATA_DEFINE_TAG_HANDLER(gpx, GPX, Name, gpxTag_nameSpace11)

#define GPX_DEFINE_TAG_HANDLER(Name) \
    GPX_DEFINE_TAG_HANDLER_10(Name) \
    GPX_DEFINE_TAG_HANDLER_11(Name)

}

#endif
