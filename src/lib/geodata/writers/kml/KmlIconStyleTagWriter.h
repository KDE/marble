//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_KMLICONSTYLETAGWRITER_H
#define MARBLE_KMLICONSTYLETAGWRITER_H

#include "GeoTagWriter.h"
#include "GeoDataHotSpot.h"
#include "KmlColorStyleTagWriter.h"

namespace Marble
{

class KmlIconStyleTagWriter : public KmlColorStyleTagWriter
{
public:
    KmlIconStyleTagWriter();

protected:
    virtual bool writeMid( const GeoNode *node, GeoWriter& writer ) const;

private:
    QString unitString( GeoDataHotSpot::Units unit ) const;
};

}

#endif
