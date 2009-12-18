//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#include "GpsdConnection.h"

#include "MarbleDebug.h"



using namespace Marble;

GpsdConnection::GpsdConnection( QObject* parent )
    : QObject( parent ),
      m_timer( 0 )
{
    gps_data_t* data = m_gpsd.open();
    if ( data ) {
#if GPSD_API_MAJOR_VERSION == 3
        m_gpsd.stream( WATCH_ENABLE );
#endif
        connect( &m_timer, SIGNAL( timeout() ), this, SLOT( update() ) );
        m_timer.start( 1000 );
    } else
        mDebug() << "Connection to gpsd failed, no position info available.";
}

void GpsdConnection::update()
{
    gps_data_t* data;
#if GPSD_API_MAJOR_VERSION == 2
    data = m_gpsd.query( "o" );
#elif GPSD_API_MAJOR_VERSION == 3
    while ((data = m_gpsd.poll()) && !(data->set & POLICY_SET)) {
        data = m_gpsd.poll();
    }
#endif
    if ( data )
        emit gpsdInfo( *data );
}



#include "GpsdConnection.moc"
