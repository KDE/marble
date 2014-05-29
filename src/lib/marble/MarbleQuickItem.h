//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Adam Dabrowski <adamdbrw@gmail.com>
//

#ifndef MARBLEQUICKITEM_H
#define MARBLEQUICKITEM_H

#include <QSharedPointer>
#include <QQuickPaintedItem>
#include "MarbleGlobal.h"

namespace Marble
{
    class MarbleModel;
    class MarbleQuickItemPrivate;

    //Class is still being developed
    class MARBLE_EXPORT MarbleQuickItem : public QQuickPaintedItem
    {
    Q_OBJECT

    public:
        MarbleQuickItem(QQuickItem *parent = 0);

    // QQuickPaintedItem interface
    public:
        void paint(QPainter *painter);

    // QQmlParserStatus interface
    public:
        void classBegin();
        void componentComplete();

    private:
        typedef QSharedPointer<MarbleQuickItemPrivate> MarbleQuickItemPrivatePtr;
        MarbleQuickItemPrivatePtr d;
        friend class MarbleQuickItemPrivate;
    };
}

#endif // MARBLEQUICKITEM_H
