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

#include "DGMLElementDictionary.h"

// One static list of all tag names, to avoid string comparisions all-over-the-place
namespace GeoDataElementDictionary {

const char* dgmlTag_nameSpace20 = "http://edu.kde.org/marble/dgml/2.0";

const char* dgmlTag_canvas = "canvas";
const char* dgmlTag_coastline = "coastline";
const char* dgmlTag_color = "color";
const char* dgmlTag_customplugin = "customplugin";
const char* dgmlTag_dem = "dem";
const char* dgmlTag_description = "description";
const char* dgmlTag_dgml = "dgml";
const char* dgmlTag_discrete = "discrete";
const char* dgmlTag_document = "document";
const char* dgmlTag_filter = "filter";
const char* dgmlTag_geodata = "geodata";
const char* dgmlTag_head = "head";
const char* dgmlTag_heading = "heading";
const char* dgmlTag_icon = "icon";
const char* dgmlTag_installmap = "installmap";
const char* dgmlTag_item = "item";
const char* dgmlTag_layer = "layer";
const char* dgmlTag_legend = "legend";
const char* dgmlTag_map = "map";
const char* dgmlTag_maximum = "maximum";
const char* dgmlTag_minimum = "minimum";
const char* dgmlTag_name = "name";
const char* dgmlTag_palette = "palette";
const char* dgmlTag_pick = "pick";
const char* dgmlTag_planet = "planet";
const char* dgmlTag_prefix = "prefix";
const char* dgmlTag_text = "text";
const char* dgmlTag_texture = "texture";
const char* dgmlTag_section = "section";
const char* dgmlTag_sourcedir = "sourcedir";
const char* dgmlTag_sourcefile = "sourcefile";
const char* dgmlTag_vector = "vector";
const char* dgmlTag_visible = "visible";
const char* dgmlTag_zoom = "zoom";

}
