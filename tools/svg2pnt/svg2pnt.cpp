//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//


#include <QCoreApplication>
#include <QDebug>

#include "svgxmlhandler.h"


int main(int argc, char *argv[])
{
    QString  sourcefile;
    QString  targetfile;

    QCoreApplication  app(argc, argv);

    for ( int i = 1; i < argc; ++i ) {
        if ( strcmp( argv[ i ], "-o" ) == 0 ) {
            targetfile = QString( argv[i+1] );
            sourcefile = QString( argv[i+2] );

            SVGXmlHandler     handler( targetfile );
            QFile             xmlFile( sourcefile );
            QXmlInputSource   inputSource(&xmlFile);
            QXmlSimpleReader  reader;

            reader.setContentHandler(&handler);
            reader.parse( inputSource );

            return 0;
        }
    }

    qDebug( " svg2pnt -o targetfile sourcefile" );
    app.exit();
}
