// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_ABSTRACTMARBLEGRAPHICSLAYOUT_H
#define MARBLE_ABSTRACTMARBLEGRAPHICSLAYOUT_H


#include "marble_export.h"

namespace Marble
{

class MarbleGraphicsItem;

class MARBLE_EXPORT AbstractMarbleGraphicsLayout
{
 public:
    AbstractMarbleGraphicsLayout();
    virtual ~AbstractMarbleGraphicsLayout();

    /**
     * This updates the positions of all items in the layout.
     * Calling this will be done on every repainting, so you won't have to do it yourself.
     */
    virtual void updatePositions( MarbleGraphicsItem *parent ) = 0;
};

}

#endif
