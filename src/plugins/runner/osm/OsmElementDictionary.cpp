// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "OsmElementDictionary.h"

namespace Marble
{
namespace osm
{

// OSM data primitives and what they represent
// https://wiki.openstreetmap.org/wiki/Elements
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
