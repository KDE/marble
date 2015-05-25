//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015 Constantin Mihalache <mihalache.c94@gmail.com>

#ifndef KMLLABELSTYLETAGWRITER_H
#define KMLLABELSTYLETAGWRITER_H

#include "KmlColorStyleTagWriter.h"
#include "GeoTagWriter.h"

namespace Marble
{

class KmlLabelStyleTagWriter : public KmlColorStyleTagWriter
{
public:
    KmlLabelStyleTagWriter();

protected:
    virtual bool writeMid( const GeoNode *node, GeoWriter &writer ) const;
    virtual bool isEmpty( const GeoNode *node ) const;
    virtual QColor defaultColor() const;
};

}
#endif // KMLLABELSTYLETAGWRITER_H
