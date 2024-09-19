// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Sanjiban Bairagya <sanjibanb@kde.org>
//

#ifndef MARBLE_DGMLRENDERORDERTAGHANDLER_H
#define MARBLE_DGMLRENDERORDERTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace dgml
{

class DgmlRenderOrderTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
