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
namespace GeoSceneElementDictionary {
    extern const char* dgmlTag_nameSpace20;

    extern const char* dgmlTag_Canvas;
    extern const char* dgmlTag_Coastline;
    extern const char* dgmlTag_Color;
    extern const char* dgmlTag_Customplugin;
    extern const char* dgmlTag_Dem;
    extern const char* dgmlTag_Description;
    extern const char* dgmlTag_Dgml;
    extern const char* dgmlTag_Discrete;
    extern const char* dgmlTag_Document;
    extern const char* dgmlTag_Filter;
    extern const char* dgmlTag_Geodata;
    extern const char* dgmlTag_Head;
    extern const char* dgmlTag_Icon;
    extern const char* dgmlTag_Installmap;
    extern const char* dgmlTag_Item;
    extern const char* dgmlTag_Layer;
    extern const char* dgmlTag_Legend;
    extern const char* dgmlTag_Map;
    extern const char* dgmlTag_Maximum;
    extern const char* dgmlTag_Minimum;
    extern const char* dgmlTag_Name;
    extern const char* dgmlTag_Palette;
    extern const char* dgmlTag_Pick;
    extern const char* dgmlTag_Prefix;
    extern const char* dgmlTag_Settings;
    extern const char* dgmlTag_Target;
    extern const char* dgmlTag_Text;
    extern const char* dgmlTag_Texture;
    extern const char* dgmlTag_Section;
    extern const char* dgmlTag_Sourcedir;
    extern const char* dgmlTag_Sourcefile;
    extern const char* dgmlTag_Vector;
    extern const char* dgmlTag_Visible;
    extern const char* dgmlTag_Zoom;
}

// Helper macro
#define DGML_DEFINE_TAG_HANDLER(Name) GEODATA_DEFINE_TAG_HANDLER(dgml, DGML, Name)

#endif // DGMLElementDictionary_h
