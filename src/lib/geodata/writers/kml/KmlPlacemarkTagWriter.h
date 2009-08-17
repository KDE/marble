//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef KMLPLACEMARKTAGWRITER_H
#define KMLPLACEMARKTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlPlacemarkTagWriter : public GeoTagWriter
{
public:
    virtual bool write( const GeoDataObject &node, GeoWriter& writer ) const;
};

}

#endif // KMLPLACEMARKTAGWRITER_H
