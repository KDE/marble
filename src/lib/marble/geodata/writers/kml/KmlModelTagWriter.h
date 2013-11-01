//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef MARBLE_KMLMODELTAGWRITER_H
#define MARBLE_KMLMODELTAGWRITER_H

#include "GeoTagWriter.h"

#include "MarbleGlobal.h"

namespace Marble
{

class KmlModelTagWriter : public GeoTagWriter
{
public:
    virtual bool write( const GeoNode *node, GeoWriter& writer ) const;
};

}

#endif
