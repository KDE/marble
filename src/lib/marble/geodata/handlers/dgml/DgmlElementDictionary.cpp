/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlElementDictionary.h"

namespace Marble
{

// One static list of all tag names, to avoid string comparisons all-over-the-place
namespace dgml
{

const char dgmlTag_nameSpace20[] = "http://edu.kde.org/marble/dgml/2.0";

const char dgmlTag_Available[] = "available";
const char dgmlTag_Blending[] = "blending";
const char dgmlTag_Brush[] = "brush";
const char dgmlTag_Center[] = "center";
const char dgmlTag_Color[] = "color";
const char dgmlTag_CustomPlugin[] = "customplugin";
const char dgmlTag_Dem[] = "dem";
const char dgmlTag_Description[] = "description";
const char dgmlTag_Dgml[] = "dgml";
const char dgmlTag_Discrete[] = "discrete";
const char dgmlTag_Document[] = "document";
const char dgmlTag_DownloadPolicy[] = "downloadPolicy";
const char dgmlTag_DownloadUrl[] = "downloadUrl";
const char dgmlTag_Filter[] = "filter";
const char dgmlTag_Geodata[] = "geodata";
const char dgmlTag_Group[] = "group";
const char dgmlTag_Head[] = "head";
const char dgmlTag_Heading[] = "heading";
const char dgmlTag_Icon[] = "icon";
const char dgmlTag_InstallMap[] = "installmap";
const char dgmlTag_Item[] = "item";
const char dgmlTag_Layer[] = "layer";
const char dgmlTag_Legend[] = "legend";
const char dgmlTag_License[] = "license";
const char dgmlTag_Map[] = "map";
const char dgmlTag_Maximum[] = "maximum";
const char dgmlTag_Minimum[] = "minimum";
const char dgmlTag_Name[] = "name";
const char dgmlTag_Palette[] = "palette";
const char dgmlTag_Pen[] = "pen";
const char dgmlTag_Projection[] = "projection";
const char dgmlTag_Property[] = "property";
const char dgmlTag_Section[] = "section";
const char dgmlTag_Settings[] = "settings";
const char dgmlTag_SourceDir[] = "sourcedir";
const char dgmlTag_SourceFile[] = "sourcefile";
const char dgmlTag_StorageLayout[] = "storageLayout";
const char dgmlTag_Target[] = "target";
const char dgmlTag_Text[] = "text";
const char dgmlTag_Texture[] = "texture";
const char dgmlTag_Theme[] = "theme";
const char dgmlTag_TileSize[] = "tileSize";
const char dgmlTag_Value[] = "value";
const char dgmlTag_Vector[] = "vector";
const char dgmlTag_Vectortile[] = "vectortile";
const char dgmlTag_Visible[] = "visible";
const char dgmlTag_Zoom[] = "zoom";
const char dgmlTag_RenderOrder[] = "renderOrder";
}
}
