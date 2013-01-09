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

#include "TextureTile.h"
#include "MarbleDebug.h"

#include <limits>

#include <QtCore/QString>

namespace Marble
{

TextureTile::TextureTile( TileId const & tileId, QImage const & image, const Blending * blending )
    : Tile( tileId ),
      m_image( image ),
      m_blending( blending )
{
    Q_ASSERT( !image.isNull() );
}

TextureTile::~TextureTile()
{
}

const char* TextureTile::nodeType() const
{
    return "TextureTile";
}

QString TextureTile::type()
{
    return "textureTile";
}

}

