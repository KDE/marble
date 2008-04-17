//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//

#include "tccore.h"

TCCoreApplication::TCCoreApplication( int argc, char ** argv ) : QCoreApplication( argc, argv )
{
    if( !(argc < 4) )
    {
        m_tilecreator = new TileCreator( argv [1], argv[2], argv[3], argv[4] );
        connect(m_tilecreator, SIGNAL(finished()), this, SLOT(quit()));
        m_tilecreator->start();
        qDebug() << m_tilecreator->isFinished() << m_tilecreator->isRunning();
        qDebug() << "test";
    }
}

TCCoreApplication::~TCCoreApplication()
{
//    if(m_tilecreator != 0) delete m_tilecreator;
}

//#include "tccore.moc"
