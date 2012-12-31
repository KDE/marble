//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Shou Ya <shouyalovelyx@gmail.com>
//

#ifndef MARBLE_KMLGROUNDOVERLAYWRITER_H
#define MARBLE_KMLGROUNDOVERLAYWRITER_H

#include "GeoTagWriter.h"

#include "MarbleGlobal.h"

namespace Marble
{

class KmlGroundOverlayWriter : public GeoTagWriter
{
public:
    virtual bool write( const GeoNode *node, GeoWriter& writer ) const;

private:
    static QString altitudeModeToString(AltitudeMode mode);
};

}

#endif

