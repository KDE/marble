//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QObject>

#include "../lib/TileCreator.h" 

class TCCoreApplication : public QCoreApplication
{
    public:
        TCCoreApplication( int argc, char ** argv );
        ~TCCoreApplication();
        
    private:
        TileCreator *m_tilecreator;
};

