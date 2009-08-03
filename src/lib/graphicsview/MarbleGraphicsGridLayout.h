//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLEGRAPHICSGRIDLAYOUT_H
#define MARBLEGRAPHICSGRIDLAYOUT_H

// Marble
#include "AbstractMarbleGraphicsLayout.h"
#include "marble_export.h"

namespace Marble
{

class ScreenGraphicsItem;
class MarbleGraphicsGridLayoutPrivate;

class MARBLE_EXPORT MarbleGraphicsGridLayout : public AbstractMarbleGraphicsLayout
{
 public:
    MarbleGraphicsGridLayout( int width, int height );

    void addItem( ScreenGraphicsItem *item, int x, int y );

    /**
     * This updates the positions of all items in the layout.
     * Calling this will be done on every repainting, so you won't have to do it yourself.
     */
    void updatePositions( MarbleGraphicsItem *parent );

 private:
    MarbleGraphicsGridLayoutPrivate * const d;
};

} // namespace Marble

#endif // MARBLEGRAPHICSGRIDLAYOUT_H
