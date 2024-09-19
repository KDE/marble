// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#ifndef KMLGXTIMESPANTAGHANDLER_H
#define KMLGXTIMESPANTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{
namespace gx
{

class KmlTimeSpanTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}
}

#endif // KMLGXTIMESPANTAGHANDLER_H
