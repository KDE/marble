//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#include "GpsdThread.h"

#include <QtCore/QMetaType>

#include "GpsdConnection.h"



using namespace Marble;

void GpsdThread::run()
{
    qRegisterMetaType<gps_data_t>( "gps_data_t" );
    GpsdConnection connection;
    connect( &connection, SIGNAL( gpsdInfo( gps_data_t ) ),
             this, SIGNAL( gpsdInfo( gps_data_t ) ) );
    exec();
}



#include "GpsdThread.moc"
