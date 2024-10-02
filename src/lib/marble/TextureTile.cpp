/*
    SPDX-FileCopyrightText: 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
    SPDX-FileCopyrightText: 2012 Ander Pijoan <ander.pijoan@deusto.es>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "TextureTile.h"
#include "MarbleDebug.h"

#include <limits>

namespace Marble
{

TextureTile::TextureTile(TileId const &tileId, QImage const &image, const Blending *blending)
    : Tile(tileId)
    , m_image(image)
    , m_blending(blending)
{
    Q_ASSERT(!image.isNull());
}

TextureTile::~TextureTile() = default;
}
