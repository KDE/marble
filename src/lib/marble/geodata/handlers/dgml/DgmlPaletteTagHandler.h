/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>
    SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>
    SPDX-FileCopyrightText: 2008 Henry de Valence <hdevalence@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_DGML_PALETTETAGHANDLER_H
#define MARBLE_DGML_PALETTETAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace dgml
{

class DgmlPaletteTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
