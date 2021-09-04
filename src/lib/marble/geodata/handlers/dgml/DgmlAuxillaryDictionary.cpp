/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
