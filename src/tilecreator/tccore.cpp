//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//

#include "tccore.h"

using namespace Marble;

TCCoreApplication::TCCoreApplication( int & argc, char ** argv ) : QCoreApplication( argc, argv )
{
    
    if( !(argc < 5) )
    {
        m_tilecreator = new TileCreator( argv [1], argv[2], argv[3], argv[4] );
        connect(m_tilecreator, SIGNAL(finished()), this, SLOT(quit()));
        m_tilecreator->start();
    }
}
