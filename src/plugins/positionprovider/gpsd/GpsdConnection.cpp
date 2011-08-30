//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
// Copyright 2011      Bastian Holst <bastianholst@gmx.de>
//

#include "GpsdConnection.h"

#include "MarbleDebug.h"

#include <QtCore/QTime>

#include <errno.h>
#include <clocale>

using namespace Marble;
/* TRANSLATOR Marble::GpsdConnection */

const int gpsUpdateInterval = 1000; // ms
const int gpsWaitTimeout = 200; // ms

GpsdConnection::GpsdConnection( QObject* parent )
    : QObject( parent ),
      m_timer( 0 )
{
    m_oldLocale = setlocale( LC_NUMERIC, NULL );
    setlocale( LC_NUMERIC, "C" );
    connect( &m_timer, SIGNAL( timeout() ), this, SLOT( update() ) );
}

GpsdConnection::~GpsdConnection()
{
    setlocale( LC_NUMERIC, m_oldLocale );
}

void GpsdConnection::initialize()
{
    m_timer.stop();
    gps_data_t* data = m_gpsd.open();
    if ( data ) {
        m_status = PositionProviderStatusAcquiring;
        emit statusChanged( m_status );

#if defined( GPSD_API_MAJOR_VERSION ) && ( GPSD_API_MAJOR_VERSION >= 3 ) && defined( WATCH_ENABLE )
        m_gpsd.stream( WATCH_ENABLE );
#endif
        m_timer.start( gpsUpdateInterval );
    }
    else {
        // There is also gps_errstr() for libgps version >= 2.90,
        // but it doesn't return a sensible error description
        switch ( errno ) {
            case NL_NOSERVICE:
                m_error = tr("Internal gpsd error (cannot get service entry)");
                break;
            case NL_NOHOST:
                m_error = tr("Internal gpsd error (cannot get host entry)");
                break;
            case NL_NOPROTO:
                m_error = tr("Internal gpsd error (cannot get protocol entry)");
                break;
            case NL_NOSOCK:
                m_error = tr("Internal gpsd error (unable to create socket)");
                break;
            case NL_NOSOCKOPT:
                m_error = tr("Internal gpsd error (unable to set socket option)");
                break;
            case NL_NOCONNECT:
                m_error = tr("No GPS device found by gpsd.");
                break;
            default:
                m_error = tr("Unknown error when opening gpsd connection");
                break;
        }

        m_status = PositionProviderStatusError;
        emit statusChanged( m_status );

        mDebug() << "Connection to gpsd failed, no position info available: " << m_error;
    }
}

void GpsdConnection::update()
{
#if defined( GPSD_API_MAJOR_VERSION ) && ( GPSD_API_MAJOR_VERSION >= 4 ) && defined( PACKET_SET )
    gps_data_t *data = 0;

    QTime watchdog;
    watchdog.start();

    while ( m_gpsd.waiting() && watchdog.elapsed() < gpsWaitTimeout ) {
        gps_data_t *currentData = m_gpsd.poll();

        if( currentData && currentData->set & PACKET_SET ) {
            data = currentData;
        }
    }

    if ( data ) {
        emit gpsdInfo( *data );
    }
#else
#if defined( GPSD_API_MAJOR_VERSION ) && ( GPSD_API_MAJOR_VERSION == 3 ) && defined( PACKET_SET )
    gps_data_t *data = m_gpsd.poll();
#else
    gps_data_t* data = m_gpsd.query( "o" );
#endif

    if ( data ) {
        emit gpsdInfo( *data );
    }
    else if ( m_status != PositionProviderStatusAcquiring ) {
        mDebug() << "Lost connection to gpsd, trying to re-open.";
        initialize();
    }
#endif
}

QString GpsdConnection::error() const
{
    return m_error;
}

#include "GpsdConnection.moc"
