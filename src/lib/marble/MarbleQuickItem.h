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
#include "GeoDataPlacemark.h"
#include "MarbleGlobal.h"

namespace Marble
{
    class MarbleModel;
    class MarbleMap;
    class MarbleQuickItemPrivate;

    //Class is still being developed
    class MARBLE_EXPORT MarbleQuickItem : public QQuickPaintedItem
    {
    Q_OBJECT

    public:
        MarbleQuickItem(QQuickItem *parent = 0);

    public slots:
        void goHome();
        void setZoom(int zoom, FlyToMode mode = Instant);
        void centerOn(const GeoDataPlacemark& placemark, bool animated = false);

        void zoomIn(FlyToMode mode = Automatic);
        void zoomOut(FlyToMode mode = Automatic);

    // QQuickPaintedItem interface
    public:
        void paint(QPainter *painter);

    // QQmlParserStatus interface
    public:
        void classBegin();
        void componentComplete();

    protected:
        MarbleModel* model();
        const MarbleModel* model() const;

        MarbleMap* map();
        const MarbleMap* map() const;

        QObject *getEventFilter() const;
        void pinch(QPointF center, qreal scale, Qt::GestureState state);

    private slots:
        void resizeMap();

    private:
        typedef QSharedPointer<MarbleQuickItemPrivate> MarbleQuickItemPrivatePtr;
        MarbleQuickItemPrivatePtr d;
        friend class MarbleQuickItemPrivate;
    };
}

#endif // MARBLEQUICKITEM_H
