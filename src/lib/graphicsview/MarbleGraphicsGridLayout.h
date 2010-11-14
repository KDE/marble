//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_MARBLEGRAPHICSGRIDLAYOUT_H
#define MARBLE_MARBLEGRAPHICSGRIDLAYOUT_H

// Marble
#include "AbstractMarbleGraphicsLayout.h"
#include "marble_export.h"

// Qt
#include <QtCore/QCoreApplication>

namespace Marble
{

class ScreenGraphicsItem;
class MarbleGraphicsGridLayoutPrivate;

class MARBLE_EXPORT MarbleGraphicsGridLayout : public AbstractMarbleGraphicsLayout
{
 public:
    MarbleGraphicsGridLayout( int rows, int columns );
    ~MarbleGraphicsGridLayout();

    void addItem( ScreenGraphicsItem *item, int row, int column );

    /**
     * This updates the positions of all items in the layout.
     * Calling this will be done on every repainting, so you won't have to do it yourself.
     */
    void updatePositions( MarbleGraphicsItem *parent );

    Qt::Alignment alignment() const;

    Qt::Alignment alignment( ScreenGraphicsItem *item ) const;

    void setAlignment( Qt::Alignment align );

    void setAlignment( ScreenGraphicsItem *item, Qt::Alignment );

    /**
     * Returns the spacing between the items inside the layout.
     */
    int spacing() const;

    /**
     * Set the spacing between the items inside the layout. By default the spacing is 0.
     */
    void setSpacing( int spacing );

 private:
    MarbleGraphicsGridLayoutPrivate * const d;
};

} // namespace Marble

#endif
