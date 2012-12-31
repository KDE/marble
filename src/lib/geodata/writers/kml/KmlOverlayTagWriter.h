//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_KMLOVERLAYTAGWRITER_H
#define MARBLE_KMLOVERLAYTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

// No registration for this writer, ColorStyle is an abstract kml element
class KmlOverlayTagWriter: public GeoTagWriter
{
public:
    KmlOverlayTagWriter( const QString &elementName );

    bool write( const GeoNode *node, GeoWriter& writer ) const;

protected:
    virtual bool writeMid( const GeoNode *node, GeoWriter& writer ) const = 0;

private:
    QString const m_elementName;
};

}

#endif
