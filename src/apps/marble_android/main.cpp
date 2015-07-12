//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015 Gábor Péterffy <peterffy95@gmail.org>
//

#include <QtGui/QApplication>
#include "MarbleWidget.h"

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    Marble::MarbleWidget * widget = new Marble::MarbleWidget;
    widget->setMapThemeId("earth/bluemarble/bluemarble.dgml");
    widget->setWindowTitle("Marble");
    widget->show();

    return app.exec();
}
