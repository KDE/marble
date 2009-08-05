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

 private:
    MarbleGraphicsGridLayoutPrivate * const d;
};

} // namespace Marble

#endif // MARBLEGRAPHICSGRIDLAYOUT_H
