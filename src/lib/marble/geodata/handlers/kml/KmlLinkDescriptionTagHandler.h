// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef KMLLINKDESCRIPTIONTAGHANDLER_H
#define KMLLINKDESCRIPTIONTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmllinkDescriptionTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
