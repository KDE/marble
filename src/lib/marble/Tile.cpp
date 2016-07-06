/*
 Copyright 2010 Jens-Michael Hoffmann <jmho@c-xx.com>

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library. If not, see <http://www.gnu.org/licenses/>.
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
