//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Shou Ya <shouyalovelyx@gmail.com>
//

#ifndef MARBLE_KMLTIMESPANWRITER_H
#define MARBLE_KMLTIMESPANWRITER_H

#include "GeoTagWriter.h"

#include "MarbleGlobal.h"

namespace Marble
{

class KmlTimeSpanWriter : public GeoTagWriter
{
public:
    bool write( const GeoNode *node, GeoWriter& writer ) const override;

};

}

#endif

