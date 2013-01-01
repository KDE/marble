//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_KMLCOLORSTYLETAGWRITER_H
#define MARBLE_KMLCOLORSTYLETAGWRITER_H

#include <QtGui/QColor>

#include "GeoTagWriter.h"

namespace Marble
{

// No registration for this writer, ColorStyle is an abstract kml element
class KmlColorStyleTagWriter: public GeoTagWriter
{
public:
    explicit KmlColorStyleTagWriter( const QString &elementName );

    bool write( const GeoNode *node, GeoWriter& writer ) const;

    static QString formatColor( const QColor &color );

protected:
    virtual bool writeMid( const GeoNode *node, GeoWriter& writer ) const = 0;

private:
    QString const m_elementName;
};

}

#endif
