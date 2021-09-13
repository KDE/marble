/*
    SPDX-FileCopyrightText: 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_DGML_AVAILABLETAGHANDLER_H
#define MARBLE_DGML_AVAILABLETAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace dgml
{

class DgmlAvailableTagHandler : public GeoTagHandler
{
 public:
    GeoNode* parse(GeoParser&) const override;
};

}
}

#endif
