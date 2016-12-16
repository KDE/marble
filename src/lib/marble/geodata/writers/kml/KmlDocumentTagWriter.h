//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef MARBLE_KMLDOCUMENTTAGWRITER_H
#define MARBLE_KMLDOCUMENTTAGWRITER_H

#include "GeoTagWriter.h"
#include "KmlFeatureTagWriter.h"

namespace Marble
{

class KmlDocumentTagWriter : public KmlFeatureTagWriter
{
public:
  KmlDocumentTagWriter();

protected:
    bool writeMid( const GeoNode *node, GeoWriter& writer ) const override;
};

}

#endif
