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

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include "../lib/tilescissor.h"
#include "../lib/TileLoader.h"

int main(int argc, char *argv[])
{
    QCoreApplication   app( argc, argv );

    if( argc < 4 )
    {
        qDebug() << "Syntax: tilecreator prefix installmap dem targetdir";
        app.exit(-1);
    }

    if ( !TileLoader::baseTilesAvailable( argv [1] ) )
    {
        TileScissor tilecreator( argv [1], argv [2], argv [3], argv [4] );
        tilecreator.createTiles();
    }

    app.exit(0);
}
