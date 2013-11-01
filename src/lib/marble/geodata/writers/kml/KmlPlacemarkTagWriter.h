//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef MARBLE_KMLPLACEMARKTAGWRITER_H
#define MARBLE_KMLPLACEMARKTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlPlacemarkTagWriter : public GeoTagWriter
{
public:
    virtual bool write( const GeoNode *node, GeoWriter& writer ) const;
};

}

#endif
