//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMTAGWRITER_H
#define MARBLE_OSMTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class OsmTagWriter : public GeoTagWriter
{
public:
    virtual bool write( const GeoNode *node, GeoWriter& writer ) const;
};

}

#endif

