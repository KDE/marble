//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_KMLGXTRACKWRITER_H
#define MARBLE_KMLGXTRACKWRITER_H

#include "GeoTagWriter.h"

namespace Marble {

class KmlTrackWriter : public GeoTagWriter
{

public:
    virtual bool write( const GeoNode *node, GeoWriter &writer ) const;
};

}

#endif // MARBLE_KMLGXTRACKWRITER_H
