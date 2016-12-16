//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_KMLLINESTYLETAGWRITER_H
#define MARBLE_KMLLINESTYLETAGWRITER_H

#include "KmlColorStyleTagWriter.h"

namespace Marble
{

class KmlLineStyleTagWriter : public KmlColorStyleTagWriter
{
public:
    KmlLineStyleTagWriter();

protected:
    bool writeMid( const GeoNode *node, GeoWriter& writer ) const override;

    bool isEmpty( const GeoNode *node ) const override;
};

}

#endif
