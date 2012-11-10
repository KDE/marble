//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Anders Lund <anders@alweb.dk>
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
    virtual GeoNode* parse(GeoParser&) const;
};

}
}

#endif
