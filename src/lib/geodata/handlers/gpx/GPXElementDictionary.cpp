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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include "GPXElementDictionary.h"

// One static list of all tag names, to avoid string comparisions all-over-the-place
namespace GeoDataElementDictionary {

const char* gpxTag_nameSpace10 = "http://www.topografix.com/GPX/1/0";
const char* gpxTag_nameSpace11 = "http://www.topografix.com/GPX/1/1";

const char* gpxTag_gpx = "gpx";

};
