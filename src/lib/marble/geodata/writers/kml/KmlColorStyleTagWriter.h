//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_KMLCOLORSTYLETAGWRITER_H
#define MARBLE_KMLCOLORSTYLETAGWRITER_H

#include <QColor>

#include "GeoTagWriter.h"

namespace Marble
{

// No registration for this writer, ColorStyle is an abstract kml element
class KmlColorStyleTagWriter: public GeoTagWriter
{
public:
    explicit KmlColorStyleTagWriter( const QString &elementName );

    bool write( const GeoNode *node, GeoWriter& writer ) const override;

    static QString formatColor( const QColor &color );

protected:
    virtual bool writeMid( const GeoNode *node, GeoWriter& writer ) const = 0;

    /** Returns true iff all properties have a default value */
    virtual bool isEmpty( const GeoNode *node ) const = 0;

    /** Returns the default color **/
    virtual QColor defaultColor() const;

private:
    QString const m_elementName;
};

}

#endif
