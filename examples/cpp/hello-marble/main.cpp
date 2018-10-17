//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#include <QApplication>

#include <marble/MarbleWidget.h>

using namespace Marble;

int main(int argc, char** argv)
{
    QApplication app(argc,argv);

    // Create a Marble QWidget without a parent
    MarbleWidget *mapWidget = new MarbleWidget();

    // Load the OpenStreetMap map
    mapWidget->setMapThemeId(QStringLiteral("earth/openstreetmap/openstreetmap.dgml"));
//    mapWidget->setMapThemeId(QStringLiteral("earth/vectorosm/vectorosm.dgml"));
    mapWidget->centerOn(11.19, 47.68);
    mapWidget->zoomView(2900);
    mapWidget->show();

    return app.exec();
}
