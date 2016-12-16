//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_KMLTRACKWRITER_H
#define MARBLE_KMLTRACKWRITER_H

#include "GeoTagWriter.h"

namespace Marble {

class KmlTrackWriter : public GeoTagWriter
{

public:
    bool write( const GeoNode *node, GeoWriter &writer ) const override;
};

}

#endif // MARBLE_KMLTRACKWRITER_H
