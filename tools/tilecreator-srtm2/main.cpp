//
// This file is part of the Marble Virtual Globe.
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
#include <MarbleDebug.h>

using namespace Marble;

int main(int argc, char *argv[])
{
    TCCoreApplication app( argc, argv );
    MarbleDebug::enable = true;
    if( argc < 2 ) {
    /*
            PREFIX: this is the prefix of the source directory
            TARGETDIR: the directory where the output should go to
            */
        qDebug() << "Syntax: tilecreator PREFIX TARGETDIR";
        return -1;
    } else {
        return app.exec();
    }
}
