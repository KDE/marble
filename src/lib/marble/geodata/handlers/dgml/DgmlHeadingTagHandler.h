/*
    SPDX-FileCopyrightText: 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
    SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_DGML_HEADINGTAGHANDLER_H
#define MARBLE_DGML_HEADINGTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace dgml
{

class DgmlHeadingTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
