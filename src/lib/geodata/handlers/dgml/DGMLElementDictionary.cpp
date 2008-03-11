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

// One static list of all tag names, to avoid string comparisons all-over-the-place
namespace GeoSceneElementDictionary {

const char* dgmlTag_nameSpace20 = "http://edu.kde.org/marble/dgml/2.0";

const char* dgmlTag_Available = "available";
const char* dgmlTag_Canvas = "canvas";
const char* dgmlTag_Coastline = "coastline";
const char* dgmlTag_Color = "color";
const char* dgmlTag_Customplugin = "customplugin";
const char* dgmlTag_Dem = "dem";
const char* dgmlTag_Description = "description";
const char* dgmlTag_Dgml = "dgml";
const char* dgmlTag_Discrete = "discrete";
const char* dgmlTag_Document = "document";
const char* dgmlTag_Enabled = "enabled";
const char* dgmlTag_Filter = "filter";
const char* dgmlTag_Geodata = "geodata";
const char* dgmlTag_Head = "head";
const char* dgmlTag_Icon = "icon";
const char* dgmlTag_Installmap = "installmap";
const char* dgmlTag_Item = "item";
const char* dgmlTag_Layer = "layer";
const char* dgmlTag_Legend = "legend";
const char* dgmlTag_Map = "map";
const char* dgmlTag_Maximum = "maximum";
const char* dgmlTag_Minimum = "minimum";
const char* dgmlTag_Name = "name";
const char* dgmlTag_Palette = "palette";
const char* dgmlTag_Pick = "pick";
const char* dgmlTag_Prefix = "prefix";
const char* dgmlTag_Property = "property";
const char* dgmlTag_Settings = "settings";
const char* dgmlTag_Target = "target";
const char* dgmlTag_Text = "text";
const char* dgmlTag_Texture = "texture";
const char* dgmlTag_Theme = "theme";
const char* dgmlTag_Section = "section";
const char* dgmlTag_Sourcedir = "sourcedir";
const char* dgmlTag_Sourcefile = "sourcefile";
const char* dgmlTag_Vector = "vector";
const char* dgmlTag_Visible = "visible";
const char* dgmlTag_Zoom = "zoom";
}
