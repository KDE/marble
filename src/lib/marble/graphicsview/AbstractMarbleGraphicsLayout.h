//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
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
