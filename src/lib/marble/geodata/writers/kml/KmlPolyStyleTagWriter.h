//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Thibaut Gridel <tgridel@free.fr>

#ifndef KMLPOLYSTYLETAGWRITER_H
#define KMLPOLYSTYLETAGWRITER_H

#include "KmlColorStyleTagWriter.h"

namespace Marble
{

class KmlPolyStyleTagWriter : public KmlColorStyleTagWriter
{
public:
    KmlPolyStyleTagWriter();

protected:
    bool writeMid( const GeoNode *node, GeoWriter& writer ) const override;
    bool isEmpty( const GeoNode *node ) const override;
};

}
#endif // KMLPOLYSTYLETAGWRITER_H
