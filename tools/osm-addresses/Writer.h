//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_WRITER_H
#define MARBLE_WRITER_H

#include "OsmRegion.h"
#include "OsmPlacemark.h"

#include <QtCore/QObject>

namespace Marble
{

class Writer : public QObject
{
public:
    explicit Writer( QObject* parent = 0 );

    virtual void addOsmRegion( const OsmRegion &region ) = 0;

    virtual void addOsmPlacemark( const OsmPlacemark &placemark ) = 0;
};

}

#endif // MARBLE_WRITER_H
