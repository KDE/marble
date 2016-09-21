//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef MARBLE_KMLPLACEMARKTAGWRITER_H
#define MARBLE_KMLPLACEMARKTAGWRITER_H

#include "KmlFeatureTagWriter.h"

namespace Marble
{

class KmlPlacemarkTagWriter : public KmlFeatureTagWriter
{
public:
  KmlPlacemarkTagWriter();

protected:
    virtual bool writeMid( const GeoNode *node, GeoWriter& writer ) const;
};

}

#endif
