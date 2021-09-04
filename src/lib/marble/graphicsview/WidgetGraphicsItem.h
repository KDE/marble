// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Bastian Holst <bastianholst@gmx.de>
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
    explicit WidgetGraphicsItem( MarbleGraphicsItem *parent = nullptr );

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
