// SPDX-FileCopyrightText: 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef MARBLE_BLENDING_H
#define MARBLE_BLENDING_H

class QImage;

namespace Marble
{
class TextureTile;

class Blending
{
public:
    virtual ~Blending();
    virtual void blend(QImage *const bottom, TextureTile const *const top) const = 0;
};

}

#endif
