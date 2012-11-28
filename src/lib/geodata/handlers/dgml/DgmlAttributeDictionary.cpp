/*
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
    Copyright (C) 2008 Torsten Rahn <tackat@kde.org>

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

#include "DgmlAttributeDictionary.h"

namespace Marble
{
namespace dgml
{

// One static list of all attribute names, to avoid string comparisons all-over-the-place

const char* dgmlAttr_nameSpace20 = "http://edu.kde.org/marble/dgml/2.0";

const char* dgmlAttr_backend          = "backend";
const char* dgmlAttr_bgcolor          = "bgcolor";
const char* dgmlAttr_color            = "color";
const char* dgmlAttr_checkable        = "checkable";
const char* dgmlAttr_connect          = "connect";
const char* dgmlAttr_expire           = "expire";
const char* dgmlAttr_feature          = "feature";
const char* dgmlAttr_format           = "format";
const char* dgmlAttr_height           = "height";
const char* dgmlAttr_host             = "host";
const char* dgmlAttr_labelColor       = "labelColor";
const char* dgmlAttr_levelZeroColumns = "levelZeroColumns";
const char* dgmlAttr_levelZeroRows    = "levelZeroRows";
const char* dgmlAttr_maximumConnections = "maximumConnections";
const char* dgmlAttr_maximumTileLevel = "maximumTileLevel";
const char* dgmlAttr_mode             = "mode";
const char* dgmlAttr_name             = "name";
const char* dgmlAttr_password         = "password";
const char* dgmlAttr_path             = "path";
const char* dgmlAttr_pixmap           = "pixmap";
const char* dgmlAttr_port             = "port";
const char* dgmlAttr_protocol         = "protocol";
const char* dgmlAttr_query            = "query";
const char* dgmlAttr_radius           = "radius";
const char* dgmlAttr_role             = "role";
const char* dgmlAttr_spacing          = "spacing";
const char* dgmlAttr_style            = "style";
const char* dgmlAttr_text             = "text";
const char* dgmlAttr_type             = "type";
const char* dgmlAttr_usage            = "usage";
const char* dgmlAttr_user             = "user";
const char* dgmlAttr_width            = "width";
}
}
