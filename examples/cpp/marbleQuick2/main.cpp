//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Adam Dabrowski <adamdbrw@gmail.com>
//

#include <QApplication>
#include <QQuickView>

#include <marble/declarative/MarbleQuickItem.h>
#include <marble/MarbleMap.h>
#include <marble/declarative/MarbleDeclarativePlugin.h>

using namespace Marble;

class MarbleDemoItem : public MarbleQuickItem
{
Q_OBJECT

public:
    MarbleDemoItem(QQuickItem *parent = 0) : MarbleQuickItem(parent)
    {
        // nothing to do
    }

    void componentComplete()
    {
        QQuickItem *pinch = findChild<QQuickItem*>(QStringLiteral("pinchArea"));
        if (pinch)
        {
            pinch->installEventFilter(getEventFilter());
        }
    }

public Q_SLOTS:

    void handlePinchStart(QPointF center)
    {
        makePinch(center, Qt::GestureStarted);
    }

    void handlePinchUpdate(QPointF center, qreal scale)
    {
        makePinch(center, Qt::GestureUpdated, scale);
    }

    void handlePinchEnd(QPointF center, bool canceled)
    {
        makePinch(center, canceled ? Qt::GestureCanceled : Qt::GestureFinished);
    }

private:
    void makePinch(QPointF center, Qt::GestureState state, qreal scale = 1)
    {
        scale = sqrt(sqrt(scale));
        scale = qBound(static_cast<qreal>(0.5), scale, static_cast<qreal>(2.0));
        pinch(center, scale, state);
    }
};

class MapTestWrap : public QQuickView
{
public:
    void start()
    {
        MarbleDeclarativePlugin plugin;
        plugin.registerTypes("org.kde.marble");
        setSource(QUrl(QStringLiteral("qrc:/main.qml")));

        if(status()!=QQuickView::Ready)
            qDebug("can't initialise view");

        QSurfaceFormat format;
        format.setAlphaBufferSize(8);
        setFormat(format);
        setClearBeforeRendering(true);
        setColor(QColor(Qt::transparent));
        setTitle(QStringLiteral("Marble in QML 2.0 demo"));

        show();
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MapTestWrap test;
    test.start();

    return app.exec();
}

#include "main.moc"
