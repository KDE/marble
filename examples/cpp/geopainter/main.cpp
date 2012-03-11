//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include <QtGui/QApplication>
#include <marble/MarbleWidget.h>
#include <marble/GeoPainter.h>

using namespace Marble;

class MyMarbleWidget : public MarbleWidget
{
public:
    virtual void customPaint(GeoPainter* painter);
};

void MyMarbleWidget::customPaint(GeoPainter* painter)
{
    GeoDataCoordinates home(8.4, 49.0, 0.0, GeoDataCoordinates::Degree);
    painter->setPen(Qt::green);
    painter->drawEllipse(home, 7, 7);
    painter->setPen(Qt::black);
    painter->drawText(home, "Hello Marble!");
}

int main(int argc, char** argv)
{
    QApplication app(argc,argv);
    MyMarbleWidget *mapWidget = new MyMarbleWidget;
    mapWidget->setMapThemeId("earth/openstreetmap/openstreetmap.dgml");
    mapWidget->show();
    return app.exec();
}
