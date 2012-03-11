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

using namespace Marble;

int main(int argc, char** argv)
{
    QApplication app(argc,argv);

    // Create a Marble QWidget without a parent
    MarbleWidget *mapWidget = new MarbleWidget();

    // Load the OpenStreetMap map
    mapWidget->setMapThemeId("earth/openstreetmap/openstreetmap.dgml");

    mapWidget->show();

    return app.exec();
}
