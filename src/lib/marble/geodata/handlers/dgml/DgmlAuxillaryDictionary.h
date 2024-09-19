/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>
    SPDX-FileCopyrightText: 2007 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
extern const char dgmlValue_true[];
extern const char dgmlValue_on[];
extern const char dgmlValue_texture[];
extern const char dgmlValue_vector[];
extern const char dgmlValue_vectortile[];
extern const char dgmlValue_geodata[];
}
}

#endif
