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

#include "GPXElementDictionary.h"

namespace Marble
{

// One static list of all tag names, to avoid string comparisons all-over-the-place
namespace gpx
{

const char* gpxTag_nameSpace10 = "http://www.topografix.com/GPX/1/0";
const char* gpxTag_nameSpace11 = "http://www.topografix.com/GPX/1/1";

const char* gpxTag_ele = "ele";
const char* gpxTag_extensions = "extensions";
const char* gpxTag_gpx = "gpx";
const char* gpxTag_lat = "lat";
const char* gpxTag_lon = "lon";
const char* gpxTag_name = "name";
const char* gpxTag_time = "time";
const char* gpxTag_desc = "desc";
const char* gpxTag_type = "type";
const char* gpxTag_trk = "trk";
const char* gpxTag_trkpt = "trkpt";
const char* gpxTag_trkseg = "trkseg";
const char* gpxTag_wpt = "wpt";
const char* gpxTag_rte = "rte";
const char* gpxTag_rtept = "rtept";
const char* gpxTag_cmt  = "cmt";
const char* gpxTag_link  = "link";
const char* gpxTag_url  = "url";
const char* gpxTag_urlname  = "urlname";

const char* gpxTag_nameSpaceGarminTrackPointExt1 = "http://www.garmin.com/xmlschemas/TrackPointExtension/v1";
const char* gpxTag_TrackPointExtension = "TrackPointExtension";
const char* gpxTag_hr = "hr";

}
}
