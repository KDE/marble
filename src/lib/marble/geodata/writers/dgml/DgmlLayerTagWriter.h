//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef DGMLLAYERTAGWRITER_H
#define DGMLLAYERTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class DgmlLayerTagWriter : public GeoTagWriter
{
public:
    virtual bool write( const GeoNode *node, GeoWriter& writer ) const;    
};

}

#endif // DGMLLAYERTAGWRITER_H
