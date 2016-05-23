//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "OsmElementDictionary.h"

namespace Marble
{
namespace osm
{

// OSM data primitives and what they represent
// http://wiki.openstreetmap.org/wiki/Elements
const char osmTag_version06[] = "0.6";

// KML to OSM translators
const char osmTranslate_document[] = "document";
const char osmTranslate_feature[] = "feature";
const char osmTranslate_placemark[] = "placemark";

// OSM writers
const char osmTag_osm[] = "osm";
const char osmTag_bound[] = "bound";
const char osmTag_bounds[] = "bounds";
const char osmTag_nd[] = "nd";
const char osmTag_node[] = "node";
const char osmTag_way[] = "way";
const char osmTag_relation[] = "relation";
const char osmTag_member[] = "member";
const char osmTag_tag[] = "tag";
}
}
