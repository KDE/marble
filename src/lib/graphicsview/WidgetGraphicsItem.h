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

// Qt
#include <QtCore/QList>

class QWidget;

namespace Marble
{

class WidgetGraphicsItemPrivate;

class MARBLE_EXPORT WidgetGraphicsItem : public ScreenGraphicsItem
{
 public:
    explicit WidgetGraphicsItem( MarbleGraphicsItem *parent = 0 );

    virtual ~WidgetGraphicsItem();

    void setWidget( QWidget *widget );
    QWidget *widget() const;

    /**
     * Paints the item in item coordinates.
     */
    virtual void paint( GeoPainter *painter, ViewportParams *viewport,
                        const QString& renderPos, GeoSceneLayer * layer = 0 );

 protected:
    virtual bool eventFilter( QObject *, QEvent * );

 private:
    Q_DISABLE_COPY( WidgetGraphicsItem )

    WidgetGraphicsItemPrivate * const d;
};

} // Namespace Marble

#endif
