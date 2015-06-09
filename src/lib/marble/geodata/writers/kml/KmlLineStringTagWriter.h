//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_KMLLINESTRINGTAGWRITER_H
#define MARBLE_KMLLINESTRINGTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlLineStringTagWriter : public GeoTagWriter
{
public:
    virtual bool write( const GeoNode *node, GeoWriter& writer ) const;
};

}

#endif
