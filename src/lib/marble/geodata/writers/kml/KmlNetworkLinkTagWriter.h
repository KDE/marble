//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Adrian Draghici <draghici.adrian.b@gmail.com>
//

#ifndef MARBLE_KMLNETWORKLINKTAGWRITER_H
#define MARBLE_KMLNETWORKLINKTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlNetworkLinkTagWriter : public GeoTagWriter
{
public:
    virtual bool write( const GeoNode *node, GeoWriter& writer ) const;
};

}

#endif
