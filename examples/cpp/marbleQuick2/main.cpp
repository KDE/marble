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
#include <QtQuick/QQuickView>
#include <QFileInfo>

#include <marble/MarbleQuickItem.h>
#include <marble/MarbleMap.h>

using namespace Marble;

class MarbleDemoItem : public MarbleQuickItem
{
Q_OBJECT

public:
    MarbleDemoItem(QQuickItem *parent = 0) : MarbleQuickItem(parent)
    {   //TODO: setters -> properties
        map()->setSize(width(), height());
        map()->setShowFrameRate(false);
        map()->setProjection(Spherical);
        map()->setMapThemeId("earth/openstreetmap/openstreetmap.dgml");
        map()->setShowAtmosphere(false);
        map()->setShowCompass(false);
        map()->setShowClouds(false);
        map()->setShowCrosshairs(false);
        map()->setShowGrid(false);
        map()->setShowOverviewMap(false);
        map()->setShowOtherPlaces(false);
        map()->setShowScaleBar(false);
        map()->setShowBackground(false);
    }

    void componentComplete()
    {
        QQuickItem *pinch = findChild<QQuickItem*>("pinchArea");
        if (pinch)
        {
            pinch->installEventFilter(getEventFilter());
        }
    }

public slots:

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
        scale = qBound(0.5, scale, 2.0);
        pinch(center, scale, state);
    }
};

class MapTestWrap : public QQuickView
{
public:
    void start()
    {
        qmlRegisterType<MarbleDemoItem>("MarbleItem", 1, 0, "MarbleItem");
        setSource(QUrl("qrc:/main.qml"));

        if(status()!=QQuickView::Ready)
            qDebug("can't initialise view");

        QSurfaceFormat format;
        format.setAlphaBufferSize(8);
        setFormat(format);
        setClearBeforeRendering(true);
        setColor(QColor(Qt::transparent));
        setTitle("Marble in QML 2.0 demo");

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
