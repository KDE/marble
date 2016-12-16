//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_WIDGETGRAPHICSITEM_H
#define MARBLE_WIDGETGRAPHICSITEM_H

// Marble
#include "ScreenGraphicsItem.h"
#include "marble_export.h"

class QWidget;

namespace Marble
{

class WidgetGraphicsItemPrivate;

class MARBLE_EXPORT WidgetGraphicsItem : public ScreenGraphicsItem
{
 public:
    explicit WidgetGraphicsItem( MarbleGraphicsItem *parent = 0 );

    ~WidgetGraphicsItem() override;

    void setWidget( QWidget *widget );
    QWidget *widget() const;

 protected:
    /**
     * Paints the item in item coordinates.
     */
    void paint( QPainter *painter ) override;

    bool eventFilter( QObject *, QEvent * ) override;

 private:
    Q_DISABLE_COPY( WidgetGraphicsItem )
    Q_DECLARE_PRIVATE(WidgetGraphicsItem)
};

} // Namespace Marble

#endif
