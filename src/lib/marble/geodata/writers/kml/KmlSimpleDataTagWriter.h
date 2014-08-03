//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#ifndef MARBLE_KMLSIMPLEDATATAGWRITER_H
#define MARBLE_KMLSIMPLEDATATAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlSimpleDataTagWriter : public GeoTagWriter
{
public:
    virtual bool write( const GeoNode *node, GeoWriter &writer ) const;
};

}

#endif
