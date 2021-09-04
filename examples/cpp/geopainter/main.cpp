//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include <QApplication>

#include <marble/MarbleWidget.h>
#include <marble/GeoPainter.h>

using namespace Marble;

class MyMarbleWidget : public MarbleWidget
{
public:
    void customPaint(GeoPainter* painter) override;
};

void MyMarbleWidget::customPaint(GeoPainter* painter)
{
    GeoDataCoordinates home(8.4, 49.0, 0.0, GeoDataCoordinates::Degree);
    painter->setPen(Qt::green);
    painter->drawEllipse(home, 7, 7);
    painter->setPen(Qt::black);
    painter->drawText(home, QStringLiteral("Hello Marble!"));
}

int main(int argc, char** argv)
{
    QApplication app(argc,argv);
    MyMarbleWidget *mapWidget = new MyMarbleWidget;
    mapWidget->setMapThemeId(QStringLiteral("earth/openstreetmap/openstreetmap.dgml"));
    mapWidget->centerOn(8.4, 49.0);
    mapWidget->show();
    return app.exec();
}
