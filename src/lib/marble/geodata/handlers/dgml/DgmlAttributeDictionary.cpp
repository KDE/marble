/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>
    SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlAttributeDictionary.h"

namespace Marble
{
namespace dgml
{

// One static list of all attribute names, to avoid string comparisons all-over-the-place

const char dgmlAttr_nameSpace20[] = "http://edu.kde.org/marble/dgml/2.0";

const char dgmlAttr_alpha[] = "alpha";
const char dgmlAttr_attribution[] = "attribution";
const char dgmlAttr_backend[] = "backend";
const char dgmlAttr_bgcolor[] = "bgcolor";
const char dgmlAttr_color[] = "color";
const char dgmlAttr_colorize[] = "colorize";
const char dgmlAttr_colorMap[] = "colorMap";
const char dgmlAttr_checkable[] = "checkable";
const char dgmlAttr_connect[] = "connect";
const char dgmlAttr_expire[] = "expire";
const char dgmlAttr_feature[] = "feature";
const char dgmlAttr_format[] = "format";
const char dgmlAttr_height[] = "height";
const char dgmlAttr_highlightBrush[] = "highlightBrush";
const char dgmlAttr_highlightPen[] = "highlightPen";
const char dgmlAttr_host[] = "host";
const char dgmlAttr_labelColor[] = "labelColor";
const char dgmlAttr_levelZeroColumns[] = "levelZeroColumns";
const char dgmlAttr_levelZeroRows[] = "levelZeroRows";
const char dgmlAttr_maximumConnections[] = "maximumConnections";
const char dgmlAttr_minimumTileLevel[] = "minimumTileLevel";
const char dgmlAttr_maximumTileLevel[] = "maximumTileLevel";
const char dgmlAttr_mode[] = "mode";
const char dgmlAttr_name[] = "name";
const char dgmlAttr_password[] = "password";
const char dgmlAttr_path[] = "path";
const char dgmlAttr_pixmap[] = "pixmap";
const char dgmlAttr_port[] = "port";
const char dgmlAttr_property[] = "property";
const char dgmlAttr_protocol[] = "protocol";
const char dgmlAttr_query[] = "query";
const char dgmlAttr_radio[] = "radio";
const char dgmlAttr_radius[] = "radius";
const char dgmlAttr_role[] = "role";
const char dgmlAttr_short[] = "short";
const char dgmlAttr_spacing[] = "spacing";
const char dgmlAttr_style[] = "style";
const char dgmlAttr_text[] = "text";
const char dgmlAttr_tileLevels[] = "tileLevels";
const char dgmlAttr_type[] = "type";
const char dgmlAttr_usage[] = "usage";
const char dgmlAttr_user[] = "user";
const char dgmlAttr_width[] = "width";
}
}
