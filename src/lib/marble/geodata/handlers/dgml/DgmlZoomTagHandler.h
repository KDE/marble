/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_DGML_ZOOMTAGHANDLER_H
#define MARBLE_DGML_ZOOMTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace dgml
{

class DgmlZoomTagHandler : public GeoTagHandler
{
 public:
    GeoNode* parse(GeoParser&) const override;
};

}
}

#endif
