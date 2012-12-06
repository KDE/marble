//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright (C) 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef MARBLE_DGML_LICENSE_TAG_HANDLER_H
#define MARBLE_DGML_LICENSE_TAG_HANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace dgml
{

class DgmlLicenseTagHandler : public GeoTagHandler
{
public:
    virtual GeoNode* parse( GeoParser &parser ) const;
};

}
}

#endif
