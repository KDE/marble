//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//

#include "tccore.h"

int main(int argc, char *argv[])
{
    TCCoreApplication app( argc, argv );
    if( argc < 4 ) {
        qDebug() << "Syntax: tilecreator PREFIX INSTALLMAP DEM TARGETDIR";
        return -1;
    } else {
        return app.exec();
    }
}
