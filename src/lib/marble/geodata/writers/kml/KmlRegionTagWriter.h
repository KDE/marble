//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mayank Madan <maddiemadan@gmail.com>
//

#ifndef MARBLE_KMLREGIONTAGWRITER_H
#define MARBLE_KMLREGIONTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlRegionTagWriter : public GeoTagWriter
{
public:
    bool write( const GeoNode *node, GeoWriter& writer ) const override;
};

}

#endif
