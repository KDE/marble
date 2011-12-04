//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Javier Becerra <javier@auva.es>
//

#ifndef MARBLE_KMLTIMESTAMPTAGWRITER_H
#define MARBLE_KMLTIMESTAMPTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlTimeStampTagWriter : public GeoTagWriter
{
public:
    virtual bool write( const GeoNode *node, GeoWriter& writer ) const;
};

}

#endif
