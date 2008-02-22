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

#ifndef DGMLElementDictionary_h
#define DGMLElementDictionary_h


// Lists all known DGML 2.0 tags
// http://edu.kde.org/marble/apis/dgml/documentation/dgmltags.html)
namespace GeoDataElementDictionary {
    extern const char* dgmlTag_nameSpace20;

    extern const char* dgmlTag_canvas;
    extern const char* dgmlTag_coastline;
    extern const char* dgmlTag_color;
    extern const char* dgmlTag_customplugin;
    extern const char* dgmlTag_dem;
    extern const char* dgmlTag_description;
    extern const char* dgmlTag_dgml;
    extern const char* dgmlTag_discrete;
    extern const char* dgmlTag_Document;
    extern const char* dgmlTag_filter;
    extern const char* dgmlTag_Folder;
    extern const char* dgmlTag_geodata;
    extern const char* dgmlTag_head;
    extern const char* dgmlTag_heading;
    extern const char* dgmlTag_icon;
    extern const char* dgmlTag_installmap;
    extern const char* dgmlTag_item;
    extern const char* dgmlTag_layer;
    extern const char* dgmlTag_legend;
    extern const char* dgmlTag_map;
    extern const char* dgmlTag_maximum;
    extern const char* dgmlTag_minimum;
    extern const char* dgmlTag_name;
    extern const char* dgmlTag_palette;
    extern const char* dgmlTag_pick;
    extern const char* dgmlTag_planet;
    extern const char* dgmlTag_prefix;
    extern const char* dgmlTag_text;
    extern const char* dgmlTag_texture;
    extern const char* dgmlTag_section;
    extern const char* dgmlTag_sourcedir;
    extern const char* dgmlTag_sourcefile;
    extern const char* dgmlTag_vector;
    extern const char* dgmlTag_visible;
    extern const char* dgmlTag_zoom;
}

// Helper macro
#define DGML_DEFINE_TAG_HANDLER(Name) GEODATA_DEFINE_TAG_HANDLER(dgml, DGML, Name)

#endif // DGMLElementDictionary_h
