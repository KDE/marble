/*
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
    Copyright (C) 2007 Torsten Rahn <rahn@kde.org>

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

#ifndef MARBLE_DGML_AUXILLARYDICTIONARY_H
#define MARBLE_DGML_AUXILLARYDICTIONARY_H

// Lists all known DGML 2.0 auxiliary strings

// This tags are compared with the backend attribute from the
// <layer> tag in the .dgml file.
// When the backend value is parsed, it is turned
// tolower so if the values in this class are not lowercase
// it wont see they are the same

namespace Marble
{
namespace dgml
{
    extern const char* dgmlValue_true;
    extern const char* dgmlValue_on;
    extern const char* dgmlValue_texture;
    extern const char* dgmlValue_vector;
    extern const char* dgmlValue_vectorTile;
    extern const char* dgmlValue_geodata;
}
}

#endif
