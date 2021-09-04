/*
    SPDX-FileCopyrightText: 2010 Jens-Michael Hoffmann <jmho@c-xx.com>

    SPDX-FileCopyrightText: 2012 Ander Pijoan <ander.pijoan@deusto.es>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "Tile.h"

#include <limits>

namespace Marble
{

Tile::Tile(TileId const & tileId)
    : m_id( tileId )
{
}

Tile::~Tile()
{
}

}
