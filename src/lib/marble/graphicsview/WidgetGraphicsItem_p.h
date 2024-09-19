// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Bastian Holst <bastianholst@gmx.de>
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
    WidgetGraphicsItemPrivate(WidgetGraphicsItem *widgetGraphicsItem, MarbleGraphicsItem *parent);

    ~WidgetGraphicsItemPrivate() override;

    QWidget *m_widget;
    QWidget *m_marbleWidget;
    QWidget *m_activeWidget;
};

}

#endif
