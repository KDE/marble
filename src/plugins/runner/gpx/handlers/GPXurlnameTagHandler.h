// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Anders Lund <anders@alweb.dk>
//

#ifndef MARBLE_GPX_URLNAMETAGHANDLER_H
#define MARBLE_GPX_URLNAMETAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace gpx
{
/// text ooccurs only in link in gpx 1.1
class GPXurlnameTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
