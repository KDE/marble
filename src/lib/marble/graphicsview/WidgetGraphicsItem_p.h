//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_WIDGETGRAPHICSITEMPRIVATE_H
#define MARBLE_WIDGETGRAPHICSITEMPRIVATE_H

#include "ScreenGraphicsItem_p.h"
#include "WidgetGraphicsItem.h"

class QWidget;

namespace Marble
{

class WidgetGraphicsItemPrivate : public ScreenGraphicsItemPrivate
{
 public:
    WidgetGraphicsItemPrivate(WidgetGraphicsItem *widgetGraphicsItem,
                              MarbleGraphicsItem *parent);

    ~WidgetGraphicsItemPrivate();
    
    QWidget *m_widget;
    QWidget *m_marbleWidget;
    QWidget *m_activeWidget;
};

}

#endif
