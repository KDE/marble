//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#ifndef KMLLATLONQUADWRITER_H
#define KMLLATLONQUADWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlLatLonQuadWriter : public GeoTagWriter
{
public:
    virtual bool write( const GeoNode *node, GeoWriter& writer ) const;
};

}


#endif // KMLLATLONQUADWRITER_H
