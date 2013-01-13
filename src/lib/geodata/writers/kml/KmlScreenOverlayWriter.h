//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mayank Madan <maddiemadan@gmail.com>
//

#ifndef MARBLE_KMLSCREENOVERLAYWRITER_H
#define MARBLE_KMLSCREENOVERLAYWRITER_H

#include "GeoTagWriter.h"
#include "GeoWriter.h"
#include "GeoDataVec2.h"
#include "KmlOverlayTagWriter.h"

#include "MarbleGlobal.h"

namespace Marble
{

class KmlScreenOverlayWriter : public KmlOverlayTagWriter
{
public:
    KmlScreenOverlayWriter();
    bool writeMid( const GeoNode *node, GeoWriter& writer ) const;

private:
    void writeVec2( const QString &element, const GeoDataVec2 &vec2, GeoWriter& writer ) const;
    QString unitToString( GeoDataVec2::Unit unit ) const;
};

}

#endif
