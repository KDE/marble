//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_KMLFEATURETAGWRITER_H
#define MARBLE_KMLFEATURETAGWRITER_H

#include "GeoTagWriter.h"

#include <QString>

namespace Marble
{

// No registration for this writer, ColorStyle is an abstract kml element
class KmlFeatureTagWriter: public GeoTagWriter
{
public:
    explicit KmlFeatureTagWriter( const QString &elementName );

    bool write( const GeoNode *node, GeoWriter& writer ) const;

protected:
    virtual bool writeMid( const GeoNode *node, GeoWriter& writer ) const = 0;

private:
    QString const m_elementName;
};

}

#endif
