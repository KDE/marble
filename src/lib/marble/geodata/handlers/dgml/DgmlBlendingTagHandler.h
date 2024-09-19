// SPDX-FileCopyrightText: 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef MARBLE_DGML_BLENDING_TAG_HANDLER_H
#define MARBLE_DGML_BLENDING_TAG_HANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace dgml
{

class DgmlBlendingTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
