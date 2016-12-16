//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Gaurav Gupta <1989.gaurav@googlemail.com>
//

#ifndef MARBLE_KMLFOLDERTAGWRITER_H
#define MARBLE_KMLFOLDERTAGWRITER_H

#include "GeoTagWriter.h"
#include "KmlFeatureTagWriter.h"

namespace Marble
{

class KmlFolderTagWriter : public KmlFeatureTagWriter
{
public:
  KmlFolderTagWriter();

protected:
    bool writeMid( const GeoNode *node, GeoWriter& writer ) const override;
};

}

#endif
