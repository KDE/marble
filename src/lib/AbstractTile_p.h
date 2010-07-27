//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009    Torsten Rahn <tackat@kde.org>
//

//
// Description: AbstractTile contains the base class for a single quadtile 
//

#ifndef MARBLE_ABSTRACTTILE_P_H
#define MARBLE_ABSTRACTTILE_P_H

#include "TileId.h"

namespace Marble
{

class AbstractTilePrivate
{
 public:
    explicit AbstractTilePrivate( const TileId& id );
    virtual ~AbstractTilePrivate();

    TileId    m_id;
    bool      m_used;

    AbstractTile * q_ptr;
};

}

#endif // MARBLE_ABSTRACTTILE_P_H
