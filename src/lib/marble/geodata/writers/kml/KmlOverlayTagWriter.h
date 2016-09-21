//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_KMLOVERLAYTAGWRITER_H
#define MARBLE_KMLOVERLAYTAGWRITER_H

#include "KmlFeatureTagWriter.h"

class QString;

namespace Marble
{

// No registration for this writer, ColorStyle is an abstract kml element
class KmlOverlayTagWriter: public KmlFeatureTagWriter
{
public:
    explicit KmlOverlayTagWriter( const QString &elementName );

protected:
    virtual bool writeMid( const GeoNode *node, GeoWriter& writer ) const = 0;
};

}

#endif
