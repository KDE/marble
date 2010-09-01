//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "routing/instructions/WaypointParser.h"
#include "routing/instructions/InstructionTransformation.h"
#include "routing/instructions/RoutingInstruction.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QLocale>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QTranslator>
#include <QtCore/QStringList>

using namespace Marble;

int main( int argc, char* argv[] )
{
    QCoreApplication app( argc, argv );
    QTranslator translator;
    translator.load( "gosmore-instructions_" + QLocale::system().name() );
    app.installTranslator( &translator );

    RoutingInstructions directions;
    WaypointParser parser;
    if ( QCoreApplication::instance()->arguments().contains( "--routino" ) )
    {
        parser.setLineSeparator( "\n" );
        parser.setFieldSeparator( '\t' );
        parser.setFieldIndex( WaypointParser::RoadName, 10 );
    }
    else
    {
        parser.addJunctionTypeMapping( "Jr", RoutingWaypoint::Roundabout );
    }

    if ( argc > 1 && !( QString( argv[argc-1] ).startsWith( "--" ) ) )
    {
        QString filename( argv[argc-1] );
        QFile input( filename );
        input.open( QIODevice::ReadOnly );
        QTextStream fileStream( &input );
        directions = InstructionTransformation::process( parser.parse( fileStream ) );
    }
    else
    {
        QTextStream console( stdin );
        directions = InstructionTransformation::process( parser.parse( console ) );
    }

    QTextStream console( stdout );
    if ( QCoreApplication::instance()->arguments().contains( "--dense" ) )
    {
        console << "Content-Type: text/plain\n\n";
    }

    for ( int i = 0; i < directions.size(); ++i )
    {
        console << directions[i] << '\n';
    }

    return 0;
}
