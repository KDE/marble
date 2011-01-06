//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Gaurav Gupta <1989.gaurav@googlemail.com>
//

#ifndef MARBLE_KMLEXTENDEDDATATAGWRITER_H
#define MARBLE_KMLEXTENDEDDATATAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlExtendedDataTagWriter : public GeoTagWriter
{
public:
    virtual bool write( const GeoNode *node, GeoWriter& writer ) const;
};

}

#endif
