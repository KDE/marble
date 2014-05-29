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

Marble::MarbleQuickItem *marble;

class MapTestWrap : public QQuickView
{
public:
    void start()
    {
        init();
    }

    void init()
    {
        setSource(QUrl("./main.qml"));

        if(status()!=QQuickView::Ready)
            qDebug("can't initialise view");

        QSurfaceFormat format;
        format.setAlphaBufferSize(8);
        setFormat(format);
        setClearBeforeRendering(true);
        setColor(QColor(Qt::transparent));
        setTitle("QML 2.0");

        marble = new Marble::MarbleQuickItem(rootObject());
        marble->setSize(QSize(800, 800));
        marble->setPosition(QPointF(0, 0));
        marble->setVisible(true);
        marble->setFocus(true);

        show();
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MapTestWrap test;
    test.start();
    QObject::connect(&test, SIGNAL(quit()), &app, SLOT(quit()));

    return app.exec();
}
