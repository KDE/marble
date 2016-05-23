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

#include "DgmlAuxillaryDictionary.h"

// One static list of all auxiliary strings, to avoid string comparisons all-over-the-place

// This tags are compared with the backend attribute from the
// <layer> tag in the .dgml file.
// When the backend value is parsed, it is turned
// tolower so if the values in this class are not lowercase
// it wont see they are the same

namespace Marble
{
namespace dgml
{
const char dgmlValue_true[] = "true";
const char dgmlValue_on[] = "on";
const char dgmlValue_texture[] = "texture";
const char dgmlValue_vector[] = "vector";
const char dgmlValue_vectortile[] = "vectortile";
const char dgmlValue_geodata[] = "geodata";
}
}
