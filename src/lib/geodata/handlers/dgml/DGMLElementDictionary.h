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

#include <marble_export.h>

// Lists all known DGML 2.0 tags
// http://edu.kde.org/marble/apis/dgml/documentation/dgmltags.html)
namespace GeoSceneElementDictionary {
    extern MARBLE_EXPORT const char* dgmlTag_nameSpace20;

    extern MARBLE_EXPORT  const char* dgmlTag_Available;
    extern MARBLE_EXPORT  const char* dgmlTag_Coastline;
    extern MARBLE_EXPORT  const char* dgmlTag_Color;
    extern MARBLE_EXPORT  const char* dgmlTag_CustomPlugin;
    extern MARBLE_EXPORT  const char* dgmlTag_Dem;
    extern MARBLE_EXPORT  const char* dgmlTag_Description;
    extern MARBLE_EXPORT  const char* dgmlTag_Discrete;
    extern MARBLE_EXPORT  const char* dgmlTag_Dgml;
    extern MARBLE_EXPORT const char* dgmlTag_Document;
    extern MARBLE_EXPORT const char* dgmlTag_Enabled;
    extern MARBLE_EXPORT const char* dgmlTag_Filter;
    extern MARBLE_EXPORT const char* dgmlTag_Geodata;
    extern MARBLE_EXPORT const char* dgmlTag_Head;
    extern MARBLE_EXPORT const char* dgmlTag_Icon;
    extern MARBLE_EXPORT const char* dgmlTag_InstallMap;
    extern MARBLE_EXPORT const char* dgmlTag_Item;
    extern MARBLE_EXPORT const char* dgmlTag_Layer;
    extern MARBLE_EXPORT const char* dgmlTag_Legend;
    extern MARBLE_EXPORT const char* dgmlTag_Map;
    extern MARBLE_EXPORT const char* dgmlTag_Maximum;
    extern MARBLE_EXPORT const char* dgmlTag_Minimum;
    extern MARBLE_EXPORT const char* dgmlTag_Name;
    extern MARBLE_EXPORT const char* dgmlTag_Palette;
    extern MARBLE_EXPORT const char* dgmlTag_Pick;
    extern MARBLE_EXPORT const char* dgmlTag_Prefix;
    extern MARBLE_EXPORT const char* dgmlTag_Property;
    extern MARBLE_EXPORT const char* dgmlTag_Section;
    extern MARBLE_EXPORT const char* dgmlTag_Settings;
    extern MARBLE_EXPORT const char* dgmlTag_SourceDir;
    extern MARBLE_EXPORT const char* dgmlTag_SourceFile;
    extern MARBLE_EXPORT const char* dgmlTag_Target;
    extern MARBLE_EXPORT const char* dgmlTag_Text;
    extern MARBLE_EXPORT const char* dgmlTag_Texture;
    extern MARBLE_EXPORT const char* dgmlTag_Theme;
    extern MARBLE_EXPORT const char* dgmlTag_Vector;
    extern MARBLE_EXPORT const char* dgmlTag_Visible;
    extern MARBLE_EXPORT const char* dgmlTag_Zoom;
}

// Helper macro
#define DGML_DEFINE_TAG_HANDLER(Name) GEODATA_DEFINE_TAG_HANDLER(dgml, DGML, Name, dgmlTag_nameSpace20)

#endif // DGMLElementDictionary_h
