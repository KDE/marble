//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include <QtGui/QApplication>

#include "QtMainWindow.h"

#if STATIC_BUILD
 #include <QtCore/QtPlugin>
 Q_IMPORT_PLUGIN(qjpeg)
 Q_IMPORT_PLUGIN(qsvg)
#endif

int main(int argc, char *argv[])
{
//    Q_INIT_RESOURCE(application);

    QApplication app(argc, argv);

    MainWindow *window = new MainWindow();
    window->resize(680, 640);
    window->show();

    return app.exec();
}
