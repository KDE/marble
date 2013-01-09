//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Thibaut Gridel <tgridel@free.fr>
//

#ifndef MARBLE_KMLMULTITRACKTAGWRITER_H
#define MARBLE_KMLMULTITRACKTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlMultiTrackTagWriter : public GeoTagWriter
{
public:
    virtual bool write( const GeoNode *node, GeoWriter& writer ) const;
};

}

#endif
