// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_MARBLEGRAPHICSGRIDLAYOUT_H
#define MARBLE_MARBLEGRAPHICSGRIDLAYOUT_H

// Marble
#include "AbstractMarbleGraphicsLayout.h"
#include "marble_export.h"

#include <Qt> // for Qt::Alignment

namespace Marble
{

class ScreenGraphicsItem;
class MarbleGraphicsGridLayoutPrivate;

class MARBLE_EXPORT MarbleGraphicsGridLayout : public AbstractMarbleGraphicsLayout
{
 public:
    MarbleGraphicsGridLayout( int rows, int columns );
    ~MarbleGraphicsGridLayout() override;

    void addItem( ScreenGraphicsItem *item, int row, int column );

    /**
     * This updates the positions of all items in the layout.
     * Calling this will be done on every repainting, so you won't have to do it yourself.
     */
    void updatePositions( MarbleGraphicsItem *parent ) override;

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
