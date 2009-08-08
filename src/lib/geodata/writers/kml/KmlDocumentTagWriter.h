//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef KMLDOCUMENTTAGWRITER_H
#define KMLDOCUMENTTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlDocumentTagWriter : public GeoTagWriter
{
public:
    virtual bool write( const GeoDataObject &node, GeoWriter& writer ) const;
};

}

#endif // KMLDOCUMENTTAGWRITER_H
